//
//  main.cpp
//  primosophie
//
//  Created by Marc Henry on 09/12/2021.
//  Copyright © 2021 Marc Henry. All rights reserved.
//

#include <iostream>
#include <sstream>      // ostringstream
#include <fstream>      // ifstream, ofstream
#include <iomanip>      // setw, setprecision
#include <unistd.h>     // getcwd, chdir
#include <cfloat>       // DBL_EPSILON, DBL_MIN, DBL_MAX
#include <vector>
#include <cmath>
#include <stdio.h>
#include <ctype.h>      // isalpha

std::string the_path;

// Handling errors during input output processing
inline int ioerror(const std::string& fn, const char *msg) {
    std::cerr << "*** File -> " << fn << " : " << msg << " ***" << std::endl;
    std::cerr << strerror(errno) << std::endl;
    return -1;
}

inline void WriteString(std::string fn, std::string buffer) {
    std::ofstream ofs(fn);
    if (ofs.good()) ofs.write(buffer.data(), buffer.size());
    else ioerror(fn, "Unable to write string buffer to file");
}

std::string FileRead(const std::string& fn)
{
    std::string buffer;
    std::ifstream ifs(fn);
    
    //    ifs.open(fn, ios_base::in);
    if (ifs.good()) buffer.assign((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
    else {
        buffer = "";
        ioerror(fn, "Open failure");
    }
    return buffer;
}


// Change current working directory according to program's full path
std::string change_cwd(const char *cmd)
{
    char buf[FILENAME_MAX];
    getcwd(buf, FILENAME_MAX-1);                        // Retrieve current working directory
    std::cout << "cwd = " << buf << std::endl;
    std::cout << "argv[0] = " << cmd << std::endl;       // Print full path name of the application
    char *cur = strcpy(buf, cmd);                       // Copy software's into the buffer
    while (*cur++);                                     // Move to end of buffer
    while (*(--cur) != '/') *cur = '\0';                // Move backwards fo erasing software's name and stops at '/' character
    chdir(buf);                                         // Change the current working directory to the new path
    return std::string(buf);                                         // Return new path
}

/* Parse a string into an array of tokens */
int parse_string(const std::string& src, std::vector<std::string>& tokens, char sep)
{
    std::string tmp;
    int i, np;
    
    // Tokenizing w.r.t. space ' '
    for (i = 0; (src[i] && isspace(src[i])); i++);      // Skip any leadin space character
    std::string clone(&src[i]);
    std::istringstream iss(clone);
    for (np = 0; getline(iss, tmp, sep); ++np) tokens.push_back(tmp);
    return np;
}

int calc(const std::string src) {
    int nbprem[] = {0, 1, 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 193, 197};
    std::string letters = "-abcdefghijklmnopqrstuvwxyz ";
    int num = static_cast<int>(src.size()), sum = 0;
    for (int i = 0; i < num; i++) {
        if (src[i] == ' ') continue;
        char tst = tolower(src[i]);
        for (int j = 0; j < 28; j++) {
            if (tst != letters[j]) continue;
            sum+= nbprem[j];
            break;
        }
    }
    return sum;
}

// Initialise scabble values
void init_scrabble(std::vector<std::string> &scrabble, std::vector<int> &vals) {
    std::vector<std::string> tokens;
    std::string words = FileRead("scrabble.txt");
    parse_string(words, tokens, '\n');
    int n = static_cast<int>(tokens.size());
    for (int i = 0; i < n; i++) {
        std::vector<std::string> toks;
        parse_string(tokens[i], toks, ' ');
        scrabble.insert(scrabble.end(), toks.begin(), toks.end());
    }
    for (int i = 0; i < static_cast<int>(scrabble.size()); i++) {
        vals.push_back(calc(scrabble[i]));
    }
}

// Convert a string representation, from ut8 to unicode
int utf8_to_unicode(std::string utf8_code)
{
    unsigned utf8_size = (unsigned) utf8_code.length();
    int unicode = 0;
    
    for (unsigned p = 0; p < utf8_size; ++p)
    {
        int bit_count = (p ? 6: 8 - utf8_size - (utf8_size == 1? 0: 1)),
        shift = (p < utf8_size - 1? (6*(utf8_size - p - 1)): 0);
        
        for (int k = 0; k < bit_count; ++k)
            unicode += ((utf8_code[p] & (1 << k)) << shift);
    }
    return unicode;
}

std::string oem(std::string& src)
{
    std::ostringstream oss;
    for (int i = 0; i < static_cast<int>(src.size());) {
        unsigned int c = static_cast<unsigned int>(static_cast<unsigned char>(src[i]));
        if (c >= 128) {
//            std::string utf8 = src.substr(i, nb);
//            int unicode = utf8_to_unicode(utf);
            if (c == 224) oss << "à";
            else if (c == 226)  oss << "å";
            else if (c == 232)  oss << "è";
            else if (c == 233)  oss << "é";
            else if (c == 234)  oss << "ê";
            else if (c == 235)  oss << "ë";
            else if (c == 238)  oss << "î";
            else if (c == 239)  oss << "ï";
            else if (c == 244)  oss << "ô";
            else if (c == 246)  oss << "ö";
            else if (c == 249)  oss << "ù";
            else if (c == 251)  oss << "û";
            else if (c == 252)  oss << "ü";
            else oss << c;
            i++;
        }
        else {
            if (src[i] == '-') oss << '-';
            else if ((ispunct(src[i]) || isspace(src[i]))) oss << ' ';   // !"#$%&'()*+-./:;<=>?@[\]^{|}'\f','\v','\n','\r','\t'
            else oss << static_cast<char>(tolower(src[i]));
            ++i;
        }
    }
    return oss.str();
}

std::string to_ascii(std::string& src)
{
    std::ostringstream oss;
    for (int i = 0; i < static_cast<int>(src.size());) {
        unsigned int c = static_cast<unsigned int>(static_cast<unsigned char>(src[i]));
        if (c < 128) {
//            if (src[i] == '\'') oss << "e ";
            if ((ispunct(src[i]) || isspace(src[i]))) oss << ' '; // !"#$%&'()*+-./:;<=>?@[\]^{|}'\f','\v','\n','\r','\t'
            else oss << static_cast<char>(tolower(src[i]));
            ++i;
        }
        else {
            int nb = (c < 224) ? 2: (c < 240) ? 3 : 4;
            std::string utf8 = src.substr(i, nb);
            int unicode = utf8_to_unicode(utf8);
            if ((unicode <= 197) && (unicode >= 192)) oss << 'a';
            else if ((unicode <= 203) && (unicode >= 200)) oss << 'e';
            else if ((unicode <= 207) && (unicode >= 204)) oss << 'i';
            else if ((unicode <= 216) && (unicode >= 210)) oss << 'o';
            else if ((unicode <= 220) && (unicode >= 217)) oss << 'u';
            else if ((unicode <= 230) && (unicode >= 224)) oss << 'a';
            else if ((unicode <= 235) && (unicode >= 232)) oss << 'e';
            else if ((unicode <= 239) && (unicode >= 236)) oss << 'i';
            else if ((unicode <= 247) && (unicode >= 242)) oss << 'o';
            else if ((unicode <= 251) && (unicode >= 248)) oss << 'u';
            else if (unicode == 199) oss << 'c';
            else if (unicode == 209) oss << 'n';
            else if (unicode == 231) oss << 'c';
            else if (unicode == 241) oss << 'n';
            else if (unicode == 339) oss << "oe";
            else if (unicode == 183) oss << '.'; //Point médian (·) = 183
            else if ((unicode == 160) || (unicode == 171) || (unicode == 187) || (unicode == 8211) || (unicode == 8226) || (unicode == 8230)) oss << ' ';
            else oss << utf8;
            //            if ((unicode == 8216) || (unicode == 8217)) oss << "e ";
            //            else if (unicode == 192)  oss << "à";
            //            else if (unicode == 201)  oss << "é";
            i+= nb;
        }
    }
    return oss.str();
}
/**
 * @brief Run Primosophie
 * 
 * @param command Entered command
 * @param tokens_vals Tokens values
 * @param Tokens Tokens strings
 * @param sentences Sentences loaded by user via "phrases.txt"
 * @param scr Scrabble words loaded via "scrabble.txt"
 * @param svals Scrabble values computed from "scrabble.txt"
 */
void run(std::string command, std::vector<int> &tokens_vals, std::vector<std::string> &Tokens, std::vector<std::string> &sentences, std::vector<std::string> &scr, std::vector<int> &svals) {

    if ((command[0] == '-') || isdigit(command[0])) {
        int val = stoi(command), n = static_cast<int>(tokens_vals.size()), nn = static_cast<int>(svals.size()), nw;
        nw = (val < 0) ? nn : n;
        val = abs(val);
        for (int i = 0; i < nw; i++) {
            if ((nw == n) && (val == tokens_vals[i])) {
                std::cout << oem(Tokens[i]) << std::endl;
            }
            else if (val == svals[i]) std::cout << scr[i] << std::endl;
        }
        for (int i = 0; i < static_cast<int>(sentences.size()); i++) {
            std::string str = to_ascii(sentences[i]);
            int code = calc(str);
            if (val == code) std::cout << sentences[i] << std::endl;
        }
    }
    else {
        std::string str = to_ascii(command);
        std::cout << "Valeur = " << calc(str) << std::endl;
    }
}

int main(int argc, const char * argv[]) {
    std::vector<std::string> tokens, Tokens, sentences, scrabble;
    std::vector<int> scrabble_vals, tokens_vals;
    
    the_path = change_cwd(argv[0]);                          // Change current working directory
    init_scrabble(scrabble, scrabble_vals);
    std::string words = FileRead("liste_fr.txt");
    for (int i = 0; i < static_cast<int>(words.size()); i++) {
        if (words[i] == '\r') words[i] = ' ';
    }
    parse_string(words, tokens, '\n');
    for (int i = 0; i < static_cast<int>(tokens.size()); i++) {
        tokens_vals.push_back(calc(tokens[i]));
    }

    parse_string(FileRead("phrases.txt"), sentences, '\n');
    std::string command;
    std::vector<std::string> cmd;
    std::cout << "Primosophie v1.0" << std::endl;
    std::cout << "Pour quitter le program, tappez entrée sans rien écrire." << std::endl;
    std::cout << "Entrez un nombre ou une phrase..." << std::endl;
    while (1) {
        std::cout << "prompt>\n";
        std::getline(std::cin, command);
        std::cout << command << std::endl;
        if (!command[0]) break;
        else if (command[0] == '+') {
            sentences.clear();
            parse_string(FileRead("phrases.txt"), sentences, '\n');
        }
         else run(command, tokens_vals, tokens, sentences, scrabble, scrabble_vals);
    }
    return 0;
}
