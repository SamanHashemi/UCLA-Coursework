/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2017 Alexander Afanasyev
 *
 * This program is free software: you can redistribute iter and/or modify iter under the terms of
 * the GNU General Public License as published by the Free Software Foundation, either version
 * 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that iter will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include "arp-cache.hpp"
#include "core/utils.hpp"
#include "core/interface.hpp"
#include "simple-router.hpp"

#include <algorithm>
#include <iostream>

namespace simple_router {
    
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    // IMPLEMENT THIS METHOD
    void
    ArpCache::periodicCheckArpRequestsAndCacheEntries()
    {
        std::list<std::shared_ptr<ArpRequest>>::iterator iter = m_arpRequests.begin();
        auto now = steady_clock::now();
        while(iter != m_arpRequests.end()) {
            if((*iter)->nTimesSent >= MAX_SENT_TIME) {
                std::cout << (*iter)->packets.size() << std::endl;
                iter = m_arpRequests.erase(iter);
            }
            else {
                std::string iface_name = (*iter)->packets.front().iface;
                const Interface *iface = m_router.findIfaceByName(iface_name);
                Buffer packetBuffer(sizeof(ethernet_hdr) + sizeof(arp_hdr));
                
                // Add all info for the Ethernet header
                ethernet_hdr* ethr_hdr = (ethernet_hdr *)packetBuffer.data();
                arp_hdr* arp_header = (arp_hdr *)((unsigned short*)packetBuffer.data() + sizeof(ethernet_hdr));
                
                memcpy(ethr_hdr->ether_shost, iface->addr.data(), 6);
                memcpy(ethr_hdr->ether_dhost, BroadcastEtherAddr, 6);
                ethr_hdr->ether_type = htons(ethertype_arp);
                
                // Add all the info for the ARP header
                arp_header->arp_hrd = htons(arp_hrd_ethernet);
                arp_header->arp_pro = htons(ethertype_ip);
                arp_header->arp_hln = 6;
                arp_header->arp_pln = 4;
                arp_header->arp_op = htons(arp_op_request);
                memcpy(arp_header->arp_sha, iface->addr.data(), 6);
                arp_header->arp_sip = iface->ip;
                memcpy(arp_header->arp_tha, BroadcastEtherAddr, 6);
                arp_header->arp_tip = (*iter)->ip;
                
                // Send out the request packet
                m_router.sendPacket(packetBuffer, iface_name);
                std::cerr << iface_name << std::endl;
                
                // Update infor
                (*iter)->nTimesSent++;
                (*iter)->timeSent = now;
                iter++;
            }
        }
        
        std::list<std::shared_ptr<ArpEntry>>::iterator entr_itr = m_cacheEntries.begin();
        while(entr_itr != m_cacheEntries.end()) {
            if((*entr_itr)->isValid) {
                entr_itr++;
            } else {
                entr_itr = m_cacheEntries.erase(entr_itr);
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    
    // You should not need to touch the rest of this code.
    
    ArpCache::ArpCache(SimpleRouter& router)
    : m_router(router)
    , m_shouldStop(false)
    , m_tickerThread(std::bind(&ArpCache::ticker, this))
    {
    }
    
    ArpCache::~ArpCache()
    {
        m_shouldStop = true;
        m_tickerThread.join();
    }
    
    std::shared_ptr<ArpEntry>
    ArpCache::lookup(uint32_t ip)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        for (const auto& entry : m_cacheEntries) {
            if (entry->isValid && entry->ip == ip) {
                return entry;
            }
        }
        
        return nullptr;
    }
    
    std::shared_ptr<ArpRequest>
    ArpCache::queueRequest(uint32_t ip, const Buffer& packet, const std::string& iface)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        auto request = std::find_if(m_arpRequests.begin(), m_arpRequests.end(),
                                    [ip] (const std::shared_ptr<ArpRequest>& request) {
                                        return (request->ip == ip);
                                    });
        
        if (request == m_arpRequests.end()) {
            request = m_arpRequests.insert(m_arpRequests.end(), std::make_shared<ArpRequest>(ip));
        }
        
        // Add the packet to the list of packets for this request
        (*request)->packets.push_back({packet, iface});
        return *request;
    }
    
    void
    ArpCache::removeRequest(const std::shared_ptr<ArpRequest>& entry)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_arpRequests.remove(entry);
    }
    
    std::shared_ptr<ArpRequest>
    ArpCache::insertArpEntry(const Buffer& mac, uint32_t ip)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        auto entry = std::make_shared<ArpEntry>();
        entry->mac = mac;
        entry->ip = ip;
        entry->timeAdded = steady_clock::now();
        entry->isValid = true;
        m_cacheEntries.push_back(entry);
        
        auto request = std::find_if(m_arpRequests.begin(), m_arpRequests.end(),
                                    [ip] (const std::shared_ptr<ArpRequest>& request) {
                                        return (request->ip == ip);
                                    });
        if (request != m_arpRequests.end()) {
            return *request;
        }
        else {
            return nullptr;
        }
    }
    
    void
    ArpCache::clear()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        m_cacheEntries.clear();
        m_arpRequests.clear();
    }
    
    void
    ArpCache::ticker()
    {
        while (!m_shouldStop) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                
                auto now = steady_clock::now();
                
                for (auto& entry : m_cacheEntries) {
                    if (entry->isValid && (now - entry->timeAdded > SR_ARPCACHE_TO)) {
                        entry->isValid = false;
                    }
                }
                
                periodicCheckArpRequestsAndCacheEntries();
            }
        }
    }
    
    std::ostream&
    operator<<(std::ostream& os, const ArpCache& cache)
    {
        std::lock_guard<std::mutex> lock(cache.m_mutex);
        
        os << "\nMAC            IP         AGE                       VALID\n"
        << "-----------------------------------------------------------\n";
        
        auto now = steady_clock::now();
        for (const auto& entry : cache.m_cacheEntries) {
            
            os << macToString(entry->mac) << "   "
            << ipToString(entry->ip) << "   "
            << std::chrono::duration_cast<seconds>((now - entry->timeAdded)).count() << " seconds   "
            << entry->isValid
            << "\n";
        }
        os << std::endl;
        return os;
    }
    
} // namespace simple_router
