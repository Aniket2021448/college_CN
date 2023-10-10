#include "byte_stream.hh"

#include <algorithm>

// You will need to add private members to the class declaration in `byte_stream.hh` -- done

/* Replace all the dummy definitions inside the methods in this file. */ 


using namespace std;

ByteStream::ByteStream(const size_t capa)
{	// A constructor called when an object has been created for it.
	capacity = capa;             // initial capcity given
	inputEnded = false;	     // initially user has not ended the input 
	_error = false;		     // also there are no errors in the begining
}

size_t ByteStream::write(const string &data) {
	
	// If there is any error before writing into the byteStream, it will return 0 as error code
	if (_error) {
		return 0;
	}

	// current bytes written
	size_t bytes_written = 0;

	// Using a for loop to traverse all data given as string format, then if current buffer filledness is less than the overall capacity alloted to it.
	// we insert data into it, else we don't insert, basically it fills till it can, after that, you cannot insert more, 
	for (char c : data) {
		if (buffer.size() < capacity) {
			buffer.push_back(c);
			bytes_written++; //current bytes
			TotalbytesWritten++;  //total bytes written
		}
		 else {
		    	break;
		}
	}

	return bytes_written; //return bytes writen as per given scenario/testcase.
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
	// handling Edge case: in case the buffer is empty or i ask for characters peek, I return empty string.
	if (_error) {
		return 0;
	}
	if (buffer.empty() || len == 0) {
		return "";
	}
	// calculating actual length of the amount of data that can be read
	// hadnling the edge case if i ask for characters more than the buffer size. In that case, we write down everything 
	// thus minimum of characters asked and current buffer length should be done, to accesss that amount of characters
	size_t actual_len = min(len, buffer.size());
	// returning just the peek of data, user is not reading or popping it out.
	string data(buffer.begin(), buffer.begin() + actual_len); 
	return data;
    
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
	// Edge case: If my ask is of 0 characters or if the buffer is empty, In this case i cannot pop out anythign, thus return from here only.
	if (_error) {
		return ;
	}
	if (len == 0 || buffer.empty()) {
		return;
	}

	// calculating actual length of the amount of data that can be read
	// hadnling the edge case if i ask for characters more than the buffer size. In that case, we write down everything 
	// thus minimum of characters asked and current buffer length should be done, to accesss that amount of characters
	size_t actual_len = min(len, buffer.size());
	// Poping the amount of characters asked.
	buffer.erase(buffer.begin(), buffer.begin() + actual_len);
	// Popping means this data has been read, but here in this function, we are just said to pop it out, not to read it, thus we do not return the string.
	TotalbytesRead += actual_len;
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {

	// We cannot read anything if our buffer is empty,
	// taking care of edge case
	if (_error) {
		return 0;
	}
	//No bytes to read, return empty
	
	if (buffer.empty()) {
		return "";
	}
	
	// calculating actual length of the amount of data that can be read
	// hadnling the edge case if i ask for characters more than the buffer size. In that case, we write down everything 
	// thus minimum of characters asked and current buffer length should be done, to accesss that amount of characters
	size_t actual_len = min(len, buffer.size());
	// buffer.begin() gives me the starting pointer to the buffer, and I want till the actual length of the ask.
	string data(buffer.begin(), buffer.begin() + actual_len);
	// As in question we have to empty that content from the buffer which has been read from the other end. thus removing it from the deque of characters Our buffer
	buffer.erase(buffer.begin(), buffer.begin() + actual_len);
	// To keep record of total bytes read, we add current read count in the last read count.
	TotalbytesRead += actual_len;
	// Thus returning The read data, in form of string
	return data;
}

// keeping record whether user has ended the input or not, if it gets triggered user cannot write more bytes in that run only.
void ByteStream::end_input() { inputEnded = true; }

// To access whether the input has ended or not
bool ByteStream::input_ended() const { return inputEnded; }

// To get the buffer size: simple c++ STL libraries 
size_t ByteStream::buffer_size() const { return buffer.size(); }

// Tells whether the buffer is empty or not
bool ByteStream::buffer_empty() const { return buffer.empty(); }

// Tells whether the deque has reached its end or not
// buffer.empty() is true in two cases: 1. if no data has been entered, 2. if all written data has been read, thus removed from the buffer
// and input ended makes sure that user is not gonna add other bytes and cannot add if it is set to true
bool ByteStream::eof() const { return buffer.empty() && inputEnded; }

// Returns the total number of bytes written
size_t ByteStream::bytes_written() const { return TotalbytesWritten; }

// Returns the total number of bytes read
size_t ByteStream::bytes_read() const { return TotalbytesRead; }

// Tells how much more we can add into the buffer
size_t ByteStream::remaining_capacity() const { return capacity - buffer.size();}

