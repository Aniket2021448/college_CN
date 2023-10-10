#include "stream_reassembler.hh"
#include <map>
// You will need to add private members to the class declaration in `stream_reassembler.hh`
using namespace std;

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity)
    :_output(capacity),
    assembledBytes(0),             //to keep track of assembled bytes
    unassembledBytes(0),           //to keep track of assembled bytes
    _capacity(capacity),           //to keep track of capacity of reassmbler/map
    nextByteNeeded(0),             //to keep track of next byte needed/ack index
    EOFindex(-1)                   //to keep track of EOF index, as came from the input ByteStream
{}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) 
{
	if(index + data.size() < nextByteNeeded)
    {   // incoming packet is completely repeated.
        return;
    }

    for (size_t i = 0;i<data.size();i++)
    {   
        storeUnassembled[index+i] = data[i]; //storing the incoming packet in the map
    }

    while (storeUnassembled.find(nextByteNeeded) != storeUnassembled.end() && _output.remaining_capacity() != 0) 
    {   
        // if we find the next byte needed in the map, we write it to the output buffer
        auto it = storeUnassembled.find(nextByteNeeded);//creating pointer to the required position
        if (it != storeUnassembled.end()) { // valid pointer check
            //creating and formatting data to store
            string dataToStore = it->second;
            _output.write(dataToStore);
            
            //Updating parameters
            storeUnassembled.erase(it);     //removing from map to drop the redundancy case
            assembledBytes ++;              //total assembled bytes
            unassembledBytes --;            //total unassembled bytes
            _capacity++;                    //capacity of map
            nextByteNeeded ++;              //next Byte to find
        }
    }

    if (eof)
    {   // if we get EOF, we store the index of EOF as index+data.size() EOF after complete packet
        EOFindex = index+data.size();
    }
    
    // if we do not get any other character to write, we end the writing process to output buffer
    if (EOFindex == nextByteNeeded)
    {   
        storeUnassembled.clear();
        _output.end_input();

    }

}

size_t StreamReassembler::unassembled_bytes() const { return storeUnassembled.size(); }

bool StreamReassembler::empty() const { return storeUnassembled.empty(); }

size_t StreamReassembler::ack_index() const { return nextByteNeeded; }