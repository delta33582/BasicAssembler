#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include "lib\binary.h"
#include "lib\assembly.h"

using namespace std;

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
            binary temp(labels[exp.at(1)]);
            temp.zero_extend(26);
            imm = temp.get_bits();
            bin_line = op + imm;
        }
        else if (type[ins] == I1)
        {
            vector<string> opr = split(exp.at(1), ',');
            op = opcode[ins];
            rs = "00000";
            rt = reg[opr.at(0)];
            binary temp(opr.at(1));
            temp.zero_extend(16);
            imm = temp.get_bits();
            bin_line = op + rs + rt + imm;
        }
        else if (type[ins] == I2)
        {
            vector<string> opr = split(exp.at(1), ',');
            op = opcode[ins];
            rs = reg[opr.at(1)];
            rt = reg[opr.at(0)];
            binary temp(opr.at(2));
            temp.sign_extend(16);
            imm = temp.get_bits();
            bin_line = op + rs + rt + imm;
        }
        else if (type[ins] == Il)
        {
            vector<string> opr = split(exp.at(1), ',');
            op = opcode[ins];
            rs = reg[opr.at(0)];
            rt = reg[opr.at(1)];
            binary temp(labels[opr.at(2)] - 1 - pc / 4);
            temp.sign_extend(16);
            imm = temp.get_bits();
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
            ofs.at(0) = (ofs.at(0) == "" ? "0" : ofs.at(0));
            binary temp(ofs.at(0));
            temp.sign_extend(16);
            imm = temp.get_bits();
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

void write(string filename, vector<string> bin_code)
{
    ofstream fout;
    fout.open(filename);

    for (string bin_line : bin_code)
    {
        fout << bin_line << "\n";
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
    write("output.bin", bin_code);

    cout << "Done!!! " << clock() - bgn << " ms.\n";

    return 0;
}