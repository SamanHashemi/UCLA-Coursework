NAME: Saman Hashemipour
UID: 904903562

High Level Design:

SimpleRouter:
handlePacket:
   We have two functions that are helping out with the handling of ARP and IPv4 packets. We check the packet and determine what kind of packet it is before we send it off to the function to be dealt with.

handleArpPacket:
   I have split this up into two smaller functions to handle replys and requests.
    handleArpReply:
	used to send the packet to the next hop
    handleArpRequest:
	create a response and send it

handleIpPacket:
    first you sanity check the packet and verify the checksum, length and TTL. Determine if it was meant for this router to drop or not and to send it to the next hop. Use the lookup function in the routing table.cpp to determine the location of the next hop
    

Problems and Solutions:
I had a difficult time figuring out how to compute and check the checksum. This was more of a comprehention issue than an actual issue with my code. I spent quite a bit of time looking for how to compute the checksum until I just re-read the spec and saw the function to do it. After that it was easy to implement it and check the checksum against the header of the packet.

I also intially had a hardtime figuring out the size of the packets because I would only make the arp related headers the size of the arp header and not the size of the two headers combine. Which was giving me a lot of errors until I later realized it and made my life a lot nicer.