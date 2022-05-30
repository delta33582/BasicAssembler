#pragma once
#include <string>
#include <sstream>

using namespace std;

class binary
{
private:
    string bits;
    int to_int(string strnum);
    void to_bin(int dec, bool sign);
    void flip();
    void set(int pos);
    void reset(int pos);
    bool test(int pos);
public:
    binary(string num);
    binary(int dec);
    ~binary();
    void sign_extend(int siz);
    void zero_extend(int siz);
    string get_bits();
};

binary::binary(string strnum)
{
    int dec = to_int(strnum);
    bool sign = (strnum.length() > 2 && strnum.at(0) == '0' && strnum.at(1) == 'x' ? false : true);
    to_bin(dec, sign);
}

binary::binary(int dec)
{
    to_bin(dec, true);
}

binary::~binary()
{
}

int binary::to_int(string strnum)
{
    int res;
    stringstream ss;
    ss << (strnum.length() > 2 && strnum.at(0) == '0' && strnum.at(1) == 'x' ? hex : dec);
    ss << strnum;
    ss >> res;
    
    return res;
}

void binary::to_bin(int dec, bool sign)
{
    bool neg = false;
    if (dec < 0)
    {
        dec *= -1;
        neg = true;
    }
    while (dec > 0)
    {
        bits = char(dec % 2 + 48) + bits;
        dec /= 2;
    }
    if (neg)
    {
        flip();
        int i = bits.length() - 1;
        
        while (test(i)) reset(i--);
        set(i);
        bits = '1' + bits;
    }
    else
    {
        if (sign == true)
        {
            bits = '0' + bits;
        }
    }
}

void binary::flip()
{
    for (char &bit : bits)
    {
        bit = (bit == '1' ? '0' : '1');
    }
}

void binary::set(int pos)
{
    bits.at(pos) = '1';
}

void binary::reset(int pos)
{
    bits.at(pos) = '0';
}


bool binary::test(int pos)
{
    return (bits.at(pos) == '1');
}

void binary::sign_extend(int siz)
{
    string ext(siz - bits.length(), bits.at(0));
    bits = ext + bits;
}

void binary::zero_extend(int siz)
{
    string ext(siz - bits.length(), '0');
    bits = ext + bits;
}

string binary::get_bits()
{
    return bits;
}