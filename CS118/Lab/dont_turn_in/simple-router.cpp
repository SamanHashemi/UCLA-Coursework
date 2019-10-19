/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2017 Alexander Afanasyev
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of
 * the GNU General Public License as published by the Free Software Foundation, either version
 * 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.ˆ
 *
 * You should have received a copy of the GNU General Public License along with this program.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include "simple-router.hpp"
#include "core/utils.hpp"

#include <fstream>

namespace simple_router {
    
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    // IMPLEMENT THIS METHOD
    
    // Improve Readability
    void
    SimpleRouter::errorExit(std::string error_msg){
        std::cerr << error_msg << std::endl;
        return;
    }
    
    void
    SimpleRouter::handlePacket(const Buffer& packet, const std::string& inIface)
    {
        std::cerr << "Got packet of size " << packet.size() << " on interface " << inIface << std::endl;
        
        // Find input network interface
        const Interface* iface = findIfaceByName(inIface);
        
        // Drop if packet is unknown
        if (iface == nullptr) {
            std::cerr << "Received packet, but interface is unknown, ignoring" << std::endl;
            return;
        }
        
        std::cerr << getRoutingTable() << std::endl;
        
        // FILL THIS IN

        // Get header info
        ethernet_hdr ethr_hdr;
        memcpy(&ethr_hdr, packet.data(), sizeof(ethernet_hdr));
        std::string pack_addr = macToString(packet);

        // Chcek if intended for router
        if ((pack_addr != macToString(iface->addr)) && (pack_addr != "ff:ff:ff:ff:ff:ff") && (pack_addr != "FF:FF:FF:FF:FF:FF") ) {
            errorExit("ERROR: This packet is not intended for this router");
            return;
        }
        uint16_t ethr_type = ethertype((const uint8_t*)packet.data());
        
        // Check the ethernet type and handle accoridngly
        if(ethr_type == ethertype_arp){ // ARP
            handleArpPacket(packet, iface);
        }
        else if(ethr_type == ethertype_ip){ // IPv4
            handleIpPacket(packet, iface);
        }
        else{ // Wasn't a correct type
            errorExit("ERROR: This packet wasn't ARP or IPv4");
            return;
        }
    
    } // End handlePacket()
    
    void
    SimpleRouter::handleArpPacket(const Buffer& packet, const Interface* iface){
        
        unsigned short a_opt;
        arp_hdr* a_hdr = (arp_hdr *)(packet.data() + sizeof(ethernet_hdr));
        a_opt = ntohs(a_hdr->arp_op);
        
        // Determine if request or reply
        if (a_opt == arp_op_request)
            handleArpRequest(a_hdr, iface);
        else if (a_opt == arp_op_reply)
            handleArpReply(a_hdr, iface);
        else
            errorExit("ERROR: ARP wasn't a request or reply");
    }
    
    void SimpleRouter::handleArpRequest(const arp_hdr* a_hdr, const Interface* iface)
    {
        if (a_hdr->arp_tip != iface->ip) {
            errorExit("ERROR: IP address doens't match");
            return;
        }
        
        Buffer packet_buf(sizeof(ethernet_hdr) + sizeof(arp_hdr));
        uint8_t* r_pack = (uint8_t *)packet_buf.data();
        
        // Create ethernet header
        ethernet_hdr* reply_eth_hdr = (ethernet_hdr *)r_pack;
        memcpy(reply_eth_hdr->ether_dhost, &(a_hdr->arp_sha), 6);
        memcpy(reply_eth_hdr->ether_shost, iface->addr.data(), 6);
        reply_eth_hdr->ether_type = htons(ethertype_arp);
        
        // Create ARP header
        arp_hdr* response_arp_hdr = (arp_hdr *)(r_pack + sizeof(ethernet_hdr));
        response_arp_hdr->arp_hrd = htons(arp_hrd_ethernet);
        response_arp_hdr->arp_pro = htons(ethertype_ip);
        response_arp_hdr->arp_hln = a_hdr->arp_hln;
        response_arp_hdr->arp_pln = a_hdr->arp_pln;
        response_arp_hdr->arp_op = htons(arp_op_reply);
        
        memcpy(response_arp_hdr->arp_sha, iface->addr.data(), 6);
        response_arp_hdr->arp_sip = iface->ip;
        memcpy(response_arp_hdr->arp_tha, &(a_hdr->arp_sha), 6);
        response_arp_hdr->arp_tip = a_hdr->arp_sip;
        
        // Send reply packet
        sendPacket(packet_buf, iface->name);
    }
    
    void SimpleRouter::handleArpReply(const arp_hdr* a_hdr, const Interface* iface)
    {
        // MAC addr
        Buffer arp_mac(6);
        memcpy(arp_mac.data(), a_hdr->arp_sha, 6);
        
        std::shared_ptr<ArpRequest> arp_request = m_arp.insertArpEntry(arp_mac, a_hdr->arp_sip);
        if (arp_request != nullptr) {
            std::list<PendingPacket>::const_iterator packet_iter = arp_request->packets.begin();
            while(packet_iter != arp_request->packets.end()){
                ethernet_hdr* ethr_hdr = (ethernet_hdr *) packet_iter->packet.data();
                
                memcpy(ethr_hdr->ether_shost, iface->addr.data(), 6);
                memcpy(ethr_hdr->ether_dhost, a_hdr->arp_sha, 6);
                
                sendPacket(packet_iter->packet, packet_iter->iface);
                packet_iter++;
            }
            m_arp.removeRequest(arp_request);
        }
    }
    
    void
    SimpleRouter::handleIpPacket(const Buffer& packet, const Interface* iface){
        
        uint16_t ip_checksum;
        Buffer ip_pack(packet);
        ip_hdr* ip_header = (ip_hdr*)(ip_pack.data() + sizeof(ethernet_hdr));
        ip_checksum = ip_header->ip_sum;
        ip_header->ip_sum = 0;
    
        // Check the length
        if ((packet.size() < (sizeof(ethernet_hdr) + sizeof(ip_hdr)))) {
            errorExit("ERROR: check IP length");
            return;
        }
        else if((ip_header->ip_len < sizeof(ip_hdr))){
            errorExit("ERROR: check IP length");
            return;
        }
        
        // Check the checksum
        if (ip_checksum != cksum(ip_header, sizeof(ip_hdr))) {
            errorExit("ERROR: Invalid ip_checksum");
            return;
        }
        
        // Check the Time To Live
        ip_header->ip_ttl -= - 1;
        if (ip_header->ip_ttl <= 0) {
            errorExit("ERROR: TTL Expired");
        }
        
        // Recompute checksum
        ip_header->ip_sum = 0;
        ip_header->ip_sum = cksum(ip_header, sizeof(ip_hdr));
        
        // Check if destined for router
        std::set<Interface>::const_iterator iface_itr = m_ifaces.begin();
        while(iface_itr != m_ifaces.end()){
            if (iface_itr->ip == ip_header->ip_dst) {
                return;
            }
            iface_itr++;
        }

        const Interface* ip_iface = findIfaceByName(m_routingTable.lookup(ip_header->ip_dst).ifName);
        
        std::shared_ptr<ArpEntry> arp_val = m_arp.lookup(ip_header->ip_dst);
        
        if(arp_val == nullptr) { // Address not in arp_val
            m_arp.queueRequest(ip_header->ip_dst, ip_pack, ip_iface->name);
            
            // Create buffer for packet
            Buffer packet_buf((uint8_t)(sizeof(ethernet_hdr) + sizeof(arp_hdr)));
            uint8_t* r_pack = (uint8_t *)packet_buf.data();
            
            // Get pointers to ethernet and arp headers
            ethernet_hdr* request_eth_hdr = (ethernet_hdr *)r_pack;
            memcpy(request_eth_hdr->ether_shost, ip_iface->addr.data(), 6);
            memcpy(request_eth_hdr->ether_dhost, BroadcastEtherAddr, 6);
            request_eth_hdr->ether_type = htons(ethertype_arp);
            
            // Make the header for the ARP
            arp_hdr* request_arp_hdr = (arp_hdr *)(r_pack + sizeof(ethernet_hdr));
            request_arp_hdr->arp_hrd = htons(arp_hrd_ethernet);
            request_arp_hdr->arp_pro = htons(ethertype_ip);
            request_arp_hdr->arp_hln = 6;
            request_arp_hdr->arp_pln = 4;
            request_arp_hdr->arp_op = htons(arp_op_request);
            memcpy(request_arp_hdr->arp_sha, ip_iface->addr.data(), 6);
            request_arp_hdr->arp_sip = ip_iface->ip;
            memcpy(request_arp_hdr->arp_tha, BroadcastEtherAddr, 6);
            request_arp_hdr->arp_tip = ip_header->ip_dst;
            
            sendPacket(packet_buf, ip_iface->name);
        }
        else{
            ethernet_hdr* ip_eth_hdr = (ethernet_hdr *)ip_pack.data();
            memcpy(ip_eth_hdr->ether_shost, ip_iface->addr.data(), 6);
            memcpy(ip_eth_hdr->ether_dhost, arp_val->mac.data(), 6);
            ip_eth_hdr->ether_type = htons(ethertype_ip);
            
            sendPacket(ip_pack, ip_iface->name);
        }
    } // end handleIpPacket()
    
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    
    // You should not need to touch the rest of this code.
    SimpleRouter::SimpleRouter()
    : m_arp(*this)
    {
    }
    
    void
    SimpleRouter::sendPacket(const Buffer& packet, const std::string& outIface)
    {
        m_pox->begin_sendPacket(packet, outIface);
    }
    
    bool
    SimpleRouter::loadRoutingTable(const std::string& rtConfig)
    {
        return m_routingTable.load(rtConfig);
    }
    
    void
    SimpleRouter::loadIfconfig(const std::string& ifconfig)
    {
        std::ifstream iff(ifconfig.c_str());
        std::string line;
        while (std::getline(iff, line)) {
            std::istringstream ifLine(line);
            std::string iface, ip;
            ifLine >> iface >> ip;
            
            in_addr ip_addr;
            if (inet_aton(ip.c_str(), &ip_addr) == 0) {
                throw std::runtime_error("Invalid IP address `" + ip + "` for interface `" + iface + "`");
            }
            
            m_ifNameToIpMap[iface] = ip_addr.s_addr;
        }
    }
    
    void
    SimpleRouter::printIfaces(std::ostream& os)
    {
        if (m_ifaces.empty()) {
            os << " Interface list empty " << std::endl;
            return;
        }
        
        for (const auto& iface : m_ifaces) {
            os << iface << "\n";
        }
        os.flush();
    }
    
    const Interface*
    SimpleRouter::findIfaceByIp(uint32_t ip) const
    {
        auto iface = std::find_if(m_ifaces.begin(), m_ifaces.end(), [ip] (const Interface& iface) {
            return iface.ip == ip;
        });
        
        if (iface == m_ifaces.end()) {
            return nullptr;
        }
        
        return &*iface;
    }
    
    const Interface*
    SimpleRouter::findIfaceByMac(const Buffer& mac) const
    {
        auto iface = std::find_if(m_ifaces.begin(), m_ifaces.end(), [mac] (const Interface& iface) {
            return iface.addr == mac;
        });
        
        if (iface == m_ifaces.end()) {
            return nullptr;
        }
        
        return &*iface;
    }
    
    const Interface*
    SimpleRouter::findIfaceByName(const std::string& name) const
    {
        auto iface = std::find_if(m_ifaces.begin(), m_ifaces.end(), [name] (const Interface& iface) {
            return iface.name == name;
        });
        
        if (iface == m_ifaces.end()) {
            return nullptr;
        }
        
        return &*iface;
    }
    
    void
    SimpleRouter::reset(const pox::Ifaces& ports)
    {
        std::cerr << "Resetting SimpleRouter with " << ports.size() << " ports" << std::endl;
        
        m_arp.clear();
        m_ifaces.clear();
        
        for (const auto& iface : ports) {
            auto ip = m_ifNameToIpMap.find(iface.name);
            if (ip == m_ifNameToIpMap.end()) {
                std::cerr << "IP_CONFIG missing information about interface `" + iface.name + "`. Skipping it" << std::endl;
                continue;
            }
            
            m_ifaces.insert(Interface(iface.name, iface.mac, ip->second));
        }
        
        printIfaces(std::cerr);
    }
    
    
} // namespace simple_router {
