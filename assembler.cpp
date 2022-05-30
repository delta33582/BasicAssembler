#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <ctime>

using namespace std;

#define R1  0
#define R2  1
#define R3  2
#define J   3
#define I1  4
#define I2  5
#define Il  6
#define Im  7
#define S   8

map<string, int> type{
    {"jr", R1},
    {"sll", R2}, {"srl", R2},
    {"add", R3}, {"addu", R3}, {"and", R3}, {"nor", R3}, {"or", R3}, {"slt", R3}, {"sltu", R3}, {"sub", R3}, {"subu", R3},
    {"j", J}, {"jal", J},
    {"lui", I1},
    {"addi", I2}, {"addiu", I2}, {"andi", I2}, {"ori", I2}, {"slti", I2}, {"sltiu", I2},
    {"beq", Il}, {"bne", Il},
    {"lbu", Im}, {"lhu", Im}, {"ll", Im}, {"lw", Im}, {"sb", Im}, {"sh", Im}, {"sw", Im},
    {"syscall", S}
};

map<string, string> funct{
    {"jr", "001000"},
    {"sll", "000000"},
    {"srl", "000010"},
    {"add", "100000"},
    {"addu", "100001"},
    {"and", "100100"},
    {"nor", "100111"},
    {"or", "100101"},
    {"slt", "101010"},
    {"sltu", "101011"},
    {"sub", "100010"},
    {"subu", "100011"}
};

map<string, string> opcode{
    {"j", "000010"},
    {"jal", "000011"},
    {"lui", "001111"},
    {"addi", "001000"},
    {"addiu", "001001"},
    {"andi", "001100"},
    {"ori", "001101"},
    {"slti", "001010"},
    {"sltiu", "001011"},
    {"beq", "000100"},
    {"bne", "000101"},
    {"lbu", "100100"},
    {"lhu", "100101"},
    {"ll", "110000"},
    {"lw", "100011"},
    {"sb", "101000"},
    {"sh", "101001"},
    {"sw", "101011"}
};

map<string, string> reg
{
    {"$0", "00000"}, {"$zero", "00000"},
    {"$1", "00001"}, {"$at", "00001"},
    {"$2", "00010"}, {"$v0", "00010"},
    {"$3", "00011"}, {"$v1", "00011"},
    {"$4", "00100"}, {"$a0", "00100"},
    {"$5", "00101"}, {"$a1", "00101"},
    {"$6", "00110"}, {"$a2", "00110"},
    {"$7", "00111"}, {"$a3", "00111"},
    {"$8", "01000"}, {"$t0", "01000"},
    {"$9", "01001"}, {"$t1", "01001"},
    {"$10", "01010"}, {"$t2", "01010"},
    {"$11", "01011"}, {"$t3", "01011"},
    {"$12", "01100"}, {"$t4", "01100"},
    {"$13", "01101"}, {"$t5", "01101"},
    {"$14", "01110"}, {"$t6", "01110"},
    {"$15", "01111"}, {"$t7", "01111"},
    {"$16", "10000"}, {"$s0", "10000"},
    {"$17", "10001"}, {"$s1", "10001"},
    {"$18", "10010"}, {"$s2", "10010"},
    {"$19", "10011"}, {"$s3", "10011"},
    {"$20", "10100"}, {"$s4", "10100"},
    {"$21", "10101"}, {"$s5", "10101"},
    {"$22", "10110"}, {"$s6", "10110"},
    {"$23", "10111"}, {"$s7", "10111"},
    {"$24", "11000"}, {"$t8", "11000"},
    {"$25", "11001"}, {"$t9", "11001"},
    {"$26", "11010"}, {"$k0", "11010"},
    {"$27", "11011"}, {"$k1", "11011"},
    {"$28", "11100"}, {"$gp", "11100"},
    {"$29", "11101"}, {"$sp", "11101"},
    {"$30", "11110"}, {"$fp", "11110"},
    {"$31", "11111"}, {"$ra", "11111"},
};

typedef string binary;

binary to_binary(int dec)
{
    bool neg = false;
    if (dec < 0)
    {
        neg = true;
        dec = dec * -1;
    }
    binary res;
    while (dec > 0)
    {
        res = char(dec % 2 + 48) + res;
        dec /= 2;
    }
    if (neg == true)
    {
        for (int i = res.length() - 1; i >= 0; i--)
        {
            res.at(i) = (res.at(i) == '0' ? '1' : '0');
        }
        for (int i = res.length() - 1; i >= 0; i--)
        {
            if (res.at(i) == '1')
            {
                res.at(i) = '0';
            }
            else
            {
                res.at(i) = '1';
                break;
            }
        }
        res = '1' + res;
    }
    else
    {
        res = '0' + res;
    }

    return res;
}

binary to_binary(string hex)
{
    binary res;
    if (hex.at(0) == '0' && hex.at(1) == 'x')
    {
        for (int i = hex.length() - 1; i > 1; i--)
        {
            if (hex.at(i) == '0')
                res = "0000" + res;
            else if (hex.at(i) == '1')
                res = "0001" + res;
            else if (hex.at(i) == '2')
                res = "0010" + res;
            else if (hex.at(i) == '3')
                res = "0011" + res;
            else if (hex.at(i) == '4')
                res = "0100" + res;
            else if (hex.at(i) == '5')
                res = "0101" + res;
            else if (hex.at(i) == '6')
                res = "0110" + res;
            else if (hex.at(i) == '7')
                res = "0111" + res;
            else if (hex.at(i) == '8')
                res = "1000" + res;
            else if (hex.at(i) == '9')
                res = "1001" + res;
            else if (hex.at(i) == 'a' || hex.at(i) == 'A')
                res = "1010" + res;
            else if (hex.at(i) == 'b' || hex.at(i) == 'B')
                res = "1011" + res;
            else if (hex.at(i) == 'c' || hex.at(i) == 'C')
                res = "1100" + res;
            else if (hex.at(i) == 'd' || hex.at(i) == 'D')
                res = "1101" + res;
            else if (hex.at(i) == 'e' || hex.at(i) == 'E')
                res = "1110" + res;
            else if (hex.at(i) == 'f' || hex.at(i) == 'F')
                res = "1111" + res;
        }
        size_t pos = res.find_first_not_of("0");
        if (pos != string::npos)
        {
            res.erase(res.begin(), res.begin() + pos);
        }
    }

    return res;
}

binary zero_extend(binary bin, int siz)
{
    binary res(siz - bin.length(), '0');
    res += bin;

    return res;
}

binary sign_extend(binary bin, int siz)
{
    binary res(siz - bin.length(), bin.at(0));
    res += bin;

    return res;
}

void simplify(string &line)
{
    size_t pos;
    pos = line.find("#");
    if (pos != string::npos)
    {
        line.erase(line.begin() + pos, line.end());
    }

    pos = line.find_first_not_of(" \t");
    if (pos != string::npos)
    {
        line.erase(line.begin(), line.begin() + pos);
    }

    pos = line.find_last_not_of(" \t");
    if (pos != string::npos)
    {
        line.erase(line.begin() + pos + 1, line.end());
    }

    for (pos = 0; line.begin() + pos < line.end();)
    {
        if (line.at(pos) == '\t' || line.at(pos) == ' ')
        {
            if (line.length() == 1 || line.at(pos + 1) == '\t'
                || line.at(pos + 1) == ' ' || line.at(pos + 1) == ','
                || (pos != 0 && line.at(pos - 1) == ':' || line.at(pos - 1) == ','))
            {
                line.erase(line.begin() + pos, line.begin() + pos + 1);
            }
            else
            {
                line.replace(line.begin() + pos, line.begin() + pos + 1, " ");
                pos++;
            }
        }
        else
        {
            pos++;
        }
    }
}

vector<string> split(string src, char delimiter)
{
    vector<string> res;
    string elm;
    size_t pre = 0;
    size_t pos = src.find(delimiter);
    while (pos != string::npos)
    {
        elm.assign(src.begin() + pre, src.begin() + pos);
        res.push_back(elm);
        pre = pos + 1;
        pos = src.find(delimiter, pre);
    }
    elm.assign(src.begin() + pre, src.end());
    res.push_back(elm);

    return res;
}

void read_and_simplify(string file_name, vector<string> &asm_code)
{
    ifstream fin;
    fin.open(file_name);
    string line;
    bool flag = true;
    while (!fin.eof())
    {
        getline(fin, line);
        if (line.find(".data") != string::npos)
        {
            flag = false;
            continue;
        }
        if (flag == false && line.find(".text") != string::npos)
        {
            flag = true;
            continue;
        }

        if (flag == true)
        {
            simplify(line);
            if (line != "")
            {
                asm_code.push_back(line);
            }
        }
    }
    fin.close();
}

void detect_label(vector<string> &asm_code, map<string, int> &labels)
{
    int pc = 0x00400000;
    for (int i = 0; i < asm_code.size(); i++)
    {
        vector<string> spllbl = split(asm_code.at(i), ':');
        
        if (spllbl.size() == 2)
        {
            labels.insert(pair<string, int>(spllbl.at(0), pc / 4));
            if (spllbl.at(1) == "")
            {
                asm_code.erase(asm_code.begin() + i);
                pc -= 4;
                i--;
            }
            else
            {
                asm_code.at(i) = spllbl.at(1);
            }
            
        }
        pc += 4;
    }
}

void binary_code_generate(vector<string> &bin_code, vector<string> asm_code, map<string, int> labels)
{
    int pc = 0x00400000;
    for (string line : asm_code)
    {
        string bin_line;
        string op;
        string rs;
        string rt;
        string rd;
        string sh;
        string fn;
        string imm;
        vector<string> exp = split(line, ' ');
        string ins = exp.at(0);
        if (type[ins] == R1)
        {
            op = "000000";
            rs = reg[exp.at(1)];
            rt = "00000";
            rd = "00000";
            sh = "00000";
            fn = funct[ins];
            bin_line = op + rs + rt + rd + sh + fn;
        }
        else if (type[ins] == R2)
        {
            vector<string> opr = split(exp.at(1), ',');
            op = "000000";
            rs = "00000";
            rt = reg[opr.at(1)];
            rd = reg[opr.at(0)];
            sh = reg[opr.at(2)];
            fn = funct[ins];
            bin_line = op + rs + rt + rd + sh + fn;
        }
        else if (type[ins] == R3)
        {
            vector<string> opr = split(exp.at(1), ',');
            op = "000000";
            rs = reg[opr.at(1)];
            rt = reg[opr.at(2)];
            rd = reg[opr.at(0)];
            sh = "00000";
            fn = funct[ins];
            bin_line = op + rs + rt + rd + sh + fn;
        }
        else if (type[ins] == J)
        {
            op = opcode[ins];
            imm = zero_extend(to_binary(labels[exp.at(1)]), 26);
            bin_line = op + imm;
        }
        else if (type[ins] == I1)
        {
            vector<string> opr = split(exp.at(1), ',');
            op = opcode[ins];
            rs = "00000";
            rt = reg[opr.at(0)];
            binary num = (opr.at(1).at(0) == '0' && opr.at(1).at(1) == 'x'
                            ? to_binary(opr.at(1))
                            : to_binary(stoi(opr.at(1))));
            imm = zero_extend(num, 16);
            bin_line = op + rs + rt + imm;
        }
        else if (type[ins] == I2)
        {
            vector<string> opr = split(exp.at(1), ',');
            op = opcode[ins];
            rs = reg[opr.at(1)];
            rt = reg[opr.at(0)];
            binary num = (opr.at(2).at(0) == '0' && opr.at(2).at(1) == 'x'
                            ? to_binary(opr.at(2))
                            : to_binary(stoi(opr.at(2))));
            imm = sign_extend(num, 16);
            bin_line = op + rs + rt + imm;
        }
        else if (type[ins] == Il)
        {
            vector<string> opr = split(exp.at(1), ',');
            op = opcode[ins];
            rs = reg[opr.at(0)];
            rt = reg[opr.at(1)];
            int addr = labels[opr.at(2)] - 1 - pc / 4;
            imm = sign_extend(to_binary(addr), 16);
            bin_line = op + rs + rt + imm;
        }
        else if (type[ins] == Im)
        {
            vector<string> opr = split(exp.at(1), ',');
            opr.at(1).erase(opr.at(1).end() - 1);
            vector<string> ofs = split(opr.at(1), '(');
            op = opcode[ins];
            rs = reg[ofs.at(1)];
            rt = reg[opr.at(0)];
            int offset = (ofs.at(0) == "" ? 0 : stoi(ofs.at(0)));
            imm = sign_extend(to_binary(offset), 16);
            bin_line = op + rs + rt + imm;
        }
        else if (type[ins] == S)
        {
            bin_line = "00000000000000000000000000001100";
        }
        bin_code.push_back(bin_line);
        pc += 4;
    }
}

void print(vector<string> asm_code)
{
    for (string line : asm_code)
    {
        cout << line << "\n";
    }
}

void write(string filename, vector<string> bin_code, vector<string> asm_code)
{
    ofstream fout;
    fout.open(filename);

    for (int i = 0; i < bin_code.size(); i++)
    {
        fout << bin_code.at(i)  << "\n";
    }

    fout.close();
}

int main()
{
    clock_t bgn = clock();

    vector<string> asm_code;
    map<string, int> labels;
    vector<string> bin_code;
    read_and_simplify("input_for_compiler.asm", asm_code);
    detect_label(asm_code, labels);
    binary_code_generate(bin_code, asm_code, labels);
    write("output.bin", bin_code, asm_code);

    cout << "Done. " << clock() - bgn << "ms.\n";

    return 0;
}