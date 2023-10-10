#include "tcp_receiver.hh"
#include <algorithm>

using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg) {
    const TCPHeader head = seg.header();   //extracts the header from it
    const auto synFlag = head.syn;         //Maintains the status of 3way handshake
    const auto TCPmessage = seg.payload(); //Stores the data with the packet
    const auto FinFlag = head.fin;         //Also known as termination/Finish flag, stating EOF

    if (synFlag) {
        //if syn flag is 1, the seqno we extract from header is actually the 
        //iniital sequence number that is isn.
        //and in order to avoid any clash in future we update the synrecieve status
        _isn = head.seqno;    
        _synReceived = true;
    }
    if (!_synReceived) {
        // We haven't received a SYN yet, so we ignore this segment.
        return;
    }
    
    // --- Hint ------
        // convert the checkPoint(seqno) into absolute seqno
        //Below line, gives the current seqno.
    uint64_t checkpoint = _reassembler.ack_index();           //given

        //As we know seqNo may wrap around in order to handle the overflow case
        //we need to unwrap the seqno, to get the absolute seqno without wrapping after fill
    uint64_t abs_seqno = unwrap(head.seqno, _isn, checkpoint); //given
    
    //we add the one ack if the falg is 1, i.e the very first connection oriented ack
    //and in case the synflag is zero, it means we can go with our usual ack numbering.
    uint64_t stream_idx = abs_seqno - _synReceived;           //given
    // --- Hint ------  

   
    //to handle the parameter mismatch, we send the copy() of the actual string view object
    //of the message/payload that came along the packet.
    //We send the copy of that object, which converts it into string, and then we can use it.
    _reassembler.push_substring(TCPmessage.copy(), stream_idx, FinFlag);

    // ... 
}

optional<WrappingInt32> TCPReceiver::ackno() const {
   
   //NOTE :: change this function
    if (!_synReceived) 
        return nullopt;

    //_reassembler.stream_out().bytes_written() tells me what is the last prooperly written
    //byte count, as the ack is:: lastSequenceNumber came + 1.

    /* IMPORTANT OBSERVATION*/
    /*  uint64_t absoluteAckNo = _reassembler.ack_index() + 1;  
        //the ack_index we recieved from reassembler might be wrapped
        //first unwrap it.
        //and since we don;t have the head/ segment we can't convert it into actual unwrapped
        //ackno using above method, instead we can use
    */
   //This method will return us the totalnumber of bytes that has been written successfully
   //in the output stream, and the ack is +1 to it.
    uint64_t ackToBeSent = _reassembler.stream_out().bytes_written() + 1;


    //this handles the last ackToBeSent sent for the last packet
    if (_reassembler.stream_out().input_ended()){
        ackToBeSent+=1;
    }
    return WrappingInt32(_isn) + ackToBeSent;
}

size_t TCPReceiver::window_size() const { return _capacity - _reassembler.stream_out().buffer_size(); }