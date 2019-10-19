void
SimpleRouter::handlePacket(const Buffer& packet, const std::string& inIface)
{
    std::cerr << "Got packet of size " << packet.size() << " on interface " << inIface << std::endl;
    
    std::cerr << std::endl << "DEBUG start of initial print_hdrs" << std::endl;
    print_hdrs(packet);
    std::cerr << "DEBUG end of initial print_hdrs" << std::endl << std::endl;
    
    const Interface* iface = findIfaceByName(inIface);
    if (iface == nullptr) {
        std::cerr << "Received packet, but interface is unknown, ignoring" << std::endl;
        return;
    }
    
    std::cerr << getRoutingTable() << std::endl;
    
    // FILL THIS IN
    
    //extract ethernet header
    ethernet_hdr* ether_header = new ethernet_hdr;
    memcpy(ether_header, packet.data(), sizeof(ethernet_hdr));
    //checking eth_type
    uint16_t ether_type = ntohs(ether_header->ether_type);
    if(ether_type == ethertype_arp){ /*its ARP*/
        //check ARP packet type
        arp_hdr* arp_header = new arp_hdr;
        memcpy(arp_header, packet.data() + sizeof(ethernet_hdr), sizeof(arp_hdr));
        unsigned short arp_opcode = ntohs(arp_header->arp_op);
        if(arp_opcode == arp_op_request){
            //ARP request
            //If the request is for incoming interface, send response
            const Interface* requested_iface = findIfaceByIp(arp_header->arp_tip);
            if(requested_iface->name == inIface){
                Buffer response_packet;
                //constructing response_eth_header
                ethernet_hdr* response_eth_header = new ethernet_hdr;
                memcpy(response_eth_header->ether_dhost, &(ether_header->ether_shost), 6);
                memcpy(response_eth_header->ether_shost, &(requested_iface->addr), 6);
                response_eth_header->ether_type = htons(ethertype_arp);
                //constructing response_arp_header
                arp_hdr* response_arp_header = new arp_hdr;
                response_arp_header->arp_hrd = arp_header->arp_hrd;
                response_arp_header->arp_pro = htons(ethertype_ip);
                response_arp_header->arp_hln = arp_header->arp_hln;
                response_arp_header->arp_pln = arp_header->arp_pln;
                response_arp_header->arp_op = htons(arp_op_reply);
                memcpy(response_arp_header->arp_sha, &(requested_iface->addr), 6);
                response_arp_header->arp_sip = requested_iface->ip;
                memcpy(response_arp_header->arp_tha, &(arp_header->arp_sha), 6);
                response_arp_header->arp_tip = arp_header->arp_sip;
                //assembling response_packet
                char* eth_array = reinterpret_cast<char*>(response_eth_header);
                response_packet.insert(response_packet.end(), eth_array, eth_array + sizeof(ethernet_hdr));
                char* arp_array = reinterpret_cast<char*>(response_arp_header);
                response_packet.insert(response_packet.end(), arp_array, arp_array + sizeof(arp_hdr));
                //send response_packet
                std::cerr << "simple_router.cpp: 84, sending arp response packet" << std::endl;
                std::cerr << std::endl << "DEBUG start of arp response print_hdrs" << std::endl;
                print_hdrs((uint8_t*)response_packet.data(), response_packet.size()); /*DEBUG*/
                std::cerr << "DEBUG end of arp response print_hdrs" << std::endl << std::endl;
                sendPacket(response_packet, inIface);
            }
        }else if(arp_opcode == arp_op_reply){
            //ARP reply
            Buffer arp_mac;
            arp_mac.insert(arp_mac.end(), arp_header->arp_sha, arp_header->arp_sha + 6);
            std::shared_ptr<ArpRequest> pending_request = m_arp.insertArpEntry(arp_mac, arp_header->arp_sip);
            //send out all pending packets if an arp request is waiting for this arp reply
            if(pending_request != nullptr){
                for(PendingPacket current_packet : pending_request->packets){
                    ethernet_hdr* modified_eth_header = new ethernet_hdr;
                    memcpy(modified_eth_header, current_packet.packet.data(), sizeof(ethernet_hdr));
                    memcpy(modified_eth_header->ether_dhost, &(arp_header->arp_sha), 6);
                    memcpy(current_packet.packet.data(), modified_eth_header, sizeof(ethernet_hdr)); /*stuff the modified header w/ correct MAC addr back into the raw packet*/
                    //send the modified packet out (w/ MAC addr from arp reply)
                    std::cerr << "simple_router.cpp: 103, (arp reply received) sending out previously queued packet to " << current_packet.iface << std::endl;
                    print_hdrs((uint8_t*)current_packet.packet.data(), current_packet.packet.size());
                    sendPacket(current_packet.packet, current_packet.iface);
                }
                //remove arp request from queue
                m_arp.removeRequest(pending_request);
            }
        }
    }else if(ether_type == ethertype_ip){ /*its IPv4*/
        //check IPv4 header fields
        ip_hdr* ipv4_header = new ip_hdr;
        memcpy(ipv4_header, packet.data() + sizeof(ethernet_hdr), sizeof(ip_hdr));
        uint16_t og_cksum = ipv4_header->ip_sum;
        ipv4_header->ip_sum = 0;
        if(ipv4_header->ip_len >= sizeof(ip_hdr) && cksum(ipv4_header, sizeof(ip_hdr)) == og_cksum){ /*checks min length and verifies cksum*/
            if(findIfaceByIp(ipv4_header->ip_dst) == nullptr){ /*packet not destined to router*/
                //decrement ttl and recompute cksum
                ipv4_header->ip_ttl = ipv4_header->ip_ttl - 1;
                ipv4_header->ip_sum = cksum(ipv4_header, sizeof(ip_hdr));
                //lookup routing table entry
                RoutingTableEntry nexthop_route_entry = m_routingTable.lookup(ipv4_header->ip_dst);
                //lookup arp entry for destination mac addr
                std::shared_ptr<ArpEntry> nexthop_arp_entry = m_arp.lookup(ipv4_header->ip_dst);
                if(nexthop_arp_entry != nullptr){
                    Buffer packet_cpy(packet);
                    //arp entry exists, send off the packet
                    memcpy(packet_cpy.data() + sizeof(ethernet_hdr), ipv4_header, sizeof(ip_hdr));  /*overwrite updated ipv4 header*/
                    memcpy(packet_cpy.data(), (nexthop_arp_entry->mac).data(), 6);                   /*fill in destination MAC addr from arp entry*/
                    const Interface* nexthop_iface = findIfaceByName(nexthop_route_entry.ifName);
                    memcpy(packet_cpy.data() + 6, (nexthop_iface->addr).data(), 6);                   /*fill in source MAC addr of next hop iface*/
                    std::cerr << "simple_router.cpp: 131, sending off ip packet w/ existing arp entry" << std::endl;
                    print_hdrs((uint8_t*)packet_cpy.data(), packet_cpy.size()); /*DEBUG*/
                    sendPacket(packet_cpy, nexthop_route_entry.ifName);
                }else{
                    //arp entry does not exist, prepare arp request
                    //construct eth header
                    ethernet_hdr* request_eth_header = new ethernet_hdr;
                    memcpy(request_eth_header->ether_dhost, BroadcastEtherAddr, 6);
                    const Interface* nexthop_iface = findIfaceByName(nexthop_route_entry.ifName);
                    memcpy(request_eth_header->ether_shost, nexthop_iface->addr.data(), 6);
                    request_eth_header->ether_type = htons(ethertype_arp);
                    //construct arp request hdr
                    arp_hdr* request_arp_header = new arp_hdr;
                    request_arp_header->arp_hrd = htons(arp_hrd_ethernet);
                    request_arp_header->arp_pro = htons(ethertype_ip);
                    request_arp_header->arp_hln = 6;
                    request_arp_header->arp_pln = 4;
                    request_arp_header->arp_op = htons(arp_op_request);
                    memcpy(request_arp_header->arp_sha, nexthop_iface->addr.data(), 6);
                    request_arp_header->arp_sip = nexthop_iface->ip;
                    memcpy(request_arp_header->arp_tha, BroadcastEtherAddr, 6);
                    request_arp_header->arp_tip = ipv4_header->ip_dst;
                    //assemble request packet
                    Buffer request_packet;
                    char* eth_array = reinterpret_cast<char*>(request_eth_header);
                    request_packet.insert(request_packet.end(), eth_array, eth_array + sizeof(ethernet_hdr));
                    char* arp_array = reinterpret_cast<char*>(request_arp_header);
                    request_packet.insert(request_packet.end(), arp_array, arp_array + sizeof(arp_hdr));
                    //send packet
                    std::cerr << "simple_router.cpp: 160, sending arp request w/o existing arp entry to " << nexthop_iface->name << std::endl;
                    print_hdrs((uint8_t*)request_packet.data(), request_packet.size()); /*DEBUG*/
                    sendPacket(request_packet, nexthop_iface->name);
                    //queue request to arp cache
                    std::shared_ptr<ArpRequest> request_ptr = m_arp.queueRequest(ipv4_header->ip_dst, packet, nexthop_iface->name);
                    //update request_ptr info
                    request_ptr->timeSent = time_point();
                    request_ptr->nTimesSent++;
                }
            }
        }
    }else{
        //ignore other types
    }
    
}
