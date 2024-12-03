#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <stdlib.h>
#include <memory>
#include <algorithm>

#include <filesystem>
namespace fs = std::filesystem;

using namespace std;


std::string trim(const std::string& str) {
    const std::string whitespace = " \t\r";
    auto begin = str.find_first_not_of(whitespace);
    if (begin == std::string::npos) begin=0;

    auto end = str.find_last_not_of(whitespace);
    string nstr =str.substr(begin, end - begin + 1);
  //  str = nstr;
    return nstr;
}
inline void sepline(const string &line,const string sep,string &partA, string &partB)
{
    size_t i = line.find(sep);
    if(i == string::npos ) {
        partA = line;
        return;
    }
    partA = line.substr(0,i);
    partB = line.substr(i+sep.length());
    trim(partA);
    trim(partB);
}
vector<string> splitt(const string &s, const string sep)
{
    vector<string> v;
    size_t i=0;
    size_t in = s.find(sep);
    do {
        v.push_back(s.substr(i,in-i));
        if(in == string::npos) break;
        i = in+sep.length();
        in = s.find(sep,i+sep.length());
    } while(i != string::npos);
    return v;
}

string rreplace(const string &s, const string orig, const string rep)
{
    stringstream ss;
    size_t i=0;
    size_t in = s.find(orig);
    do {
        ss << s.substr(i,in-i);
        if(in == string::npos) break;
        i = in+orig.length();
        ss << rep;
        in = s.find(orig,i+orig.length());
    } while(i != string::npos);
    return ss.str();
}
vector<string> splitt_trim(const string &s, const string sep)
{
    vector<string> v;
    size_t i=0;
    size_t in = s.find(sep);
    do {
        string ss = s.substr(i,in-i);
        ss = rreplace(ss,"\r","");
        ss = rreplace(ss,"\n","");
        ss = trim(ss);
        v.push_back(ss);
        if(in == string::npos) break;
        i = in+sep.length();
        in = s.find(sep,i+sep.length());
    } while(i != string::npos);
    return v;
}

// caution, sucks.
string trimquotes(std::string s)
{
    trim(s);
    rreplace(s,"\"","");
    return s;
}
void toUpper(string &s) {
    transform(s.begin(), s.end(), s.begin(),
              ::toupper);
}

string rgetline(istream &ifs)
{
    string s;
    getline(ifs,s);
    if(s.length()>0 && s.back()=='\r') s.pop_back();
    return s;
}



struct paragr {
    int _level;
    bool _isdot=false;
    string _tit;
    vector<string> _t;

    vector<paragr> _prgs;
};

class Agdoc {
public:
    void treatmdfile( string &mdpath);

    void export_ag(ofstream &ofs);
    vector< shared_ptr<paragr> > _prgs;

};
bool startWith(const string &s,const string &t)
{
    return(s.find(t) == 0);
}

void Agdoc::treatmdfile(  string &mdpath)
{
    ifstream ifsmd(mdpath);
    if(!ifsmd.good()) return;

    shared_ptr<paragr> curprg = make_shared<paragr>();
    _prgs.push_back(curprg);

    string l;
    getline(ifsmd,l);
    while(!ifsmd.eof())
    {
       l = trim(l);
       if(l.length()==0 )
       {
        getline(ifsmd,l);
        continue;
       }
       if(startWith(l,"###"))
       {
           curprg = make_shared<paragr>();
            _prgs.push_back(curprg);

            curprg->_tit = trim(l.substr(3));
            curprg->_level = 3;
       } else if(startWith(l,"##"))
       {
           curprg = make_shared<paragr>();
            _prgs.push_back(curprg);

            curprg->_tit = trim(l.substr(2));
            curprg->_level = 2;
       } else if(startWith(l,"#"))
       {
           curprg = make_shared<paragr>();
            _prgs.push_back(curprg);

            curprg->_tit = trim(l.substr(1));
            curprg->_level = 1;
       } else if(startWith(l,"**"))
       {
            curprg->_t.push_back(l);
       }else if(startWith(l,"*"))
       {
           curprg = make_shared<paragr>();
            _prgs.push_back(curprg);
            curprg->_isdot = true;
            curprg->_t.push_back( l);

       }else if(startWith(l,"-"))
       {
           curprg = make_shared<paragr>();
            _prgs.push_back(curprg);
            curprg->_isdot = true;
            curprg->_t.push_back(l);
       } else
       {
            curprg->_t.push_back(l);
       }

        getline(ifsmd,l);
    }
}
void toLower(string &s) {
    transform(s.begin(), s.end(), s.begin(),
              ::tolower);
}

string titleToMdId(string t)
{
    string trimt = trim(t);
 trimt = rreplace(trimt," ","-");
 toLower(trimt);
 return trimt;
}

string replaceLinks(const string s)
{
    // [Prerequesite and installation](#prerequesite-and-installation])
    bool inpa=false,inpb=false;
    size_t l = s.length() ;
    size_t i=0,iprev=0;
    string final;
    while(i<l)
    {
       i = s.find("[",iprev);
       if(i== string::npos) break;
       size_t ib = s.find("]",i+1);
        if(ib== string::npos) break;

        size_t ic = s.find("(",ib+1);
        if(ic== string::npos) break;
        size_t id = s.find(")",ic+1);
        if(id== string::npos) break;

         string visible = s.substr(i+1,ib-(i+1));
         string link = s.substr(ic+1,id-(ic+1));
         link = rreplace(link,"#","");

        final += s.substr(iprev,(i== string::npos)?i:(i-iprev));
        final += "@{\" ";
        final += visible;
        final += " \" LINK ";
        final +=link;
        final += "}";
        iprev = id+1;
        i=iprev;
    }
    final += s.substr(iprev,(i== string::npos)?i:(i-iprev));
    return final;
}
//  ![Game driver list interface](guideimg/mmcontrols)
string replaceImg(const string s)
{

    bool inpa=false,inpb=false;
    size_t l = s.length() ;
    size_t i=0,iprev=0;
//    string final;
    stringstream ssfinal;
    while(i<l)
    {
       i = s.find("![",iprev);
       if(i== string::npos) break;
       size_t ib = s.find("]",i+2);
        if(ib== string::npos) break;

        size_t ic = s.find("(",ib+1);
        if(ic== string::npos) break;
        size_t id = s.find(")",ic+1);
        if(id== string::npos) break;

         string visible = s.substr(i+2,ib-(i+2));
         string link = s.substr(ic+1,id-(ic+1));
         link = rreplace(link,"#","");

        ssfinal << s.substr(iprev,(i== string::npos)?i:(i-iprev));
// @{"Picture of a Workbench Screen" SYSTEM sys:utilities/Display sys:Workbench.pic}
        ssfinal << "@{\" " << visible << " \" SYSTEM multiview "<<link<<".gif "<< "}" ;

        iprev = id+1;
        i=iprev;
    }
    ssfinal << s.substr(iprev,(i== string::npos)?i:(i-iprev));
    return ssfinal.str();
}


string replaceBolds(const string s)
{
    stringstream ss;
    size_t i=0;
    bool isin=false;
    string orig = "**";
    size_t in = s.find(orig);
    do {
        ss << s.substr(i,in-i);
        if(in == string::npos) break;
        isin = !isin;

        i = in+2;
        ss << ((isin)?"@{b}":"@{ub}");
        in = s.find(orig,i+orig.length());
    } while(i != string::npos);
    return ss.str();
}
void Agdoc::export_ag(ofstream &ofs)
{
    ofs << "@DATABASE\n";
 bool nodeison=false;
 bool isFirstNode=true;
 vector<shared_ptr<paragr>>::iterator it = _prgs.begin();

    int dotindex=0; // dots are paragraphs
 while(it != _prgs.end() )
 {
    shared_ptr<paragr> p = *it++;
    if(p->_level>0) {

        if(nodeison) ofs << "\n@ENDNODE\n";
         string nodename= titleToMdId(p->_tit);
         if(isFirstNode) nodename = "MAIN";
         isFirstNode = false;

         ofs << "@NODE "<<nodename<<" \""<< p->_tit <<"\"\n";

//         ofs << "@WORDWRAP\n@Width 74\n@TAB 2\n";
            ofs << "@SMARTWRAP\n@TAB 3\n";

//@SMARTWRAP
//@TAB 3
/*
@DATABASE "MMU"
@$VER: MMU.guide 1.03 (30.10.99)
@(C) THOR Software
@SMARTWRAP
@AUTHOR Thomas Richter
@INDEX Index
@NODE MAIN "MMU Guide"
@{CODE}
...
@{BODY}


@TOC Glossary


Attribute Commands
******************

Attribute Commands
==================

@{b}{<label> <command>}@{ub}
     Specify a hypertext link.  Not really an attribute per se, but shares a
     similar syntax and scope.  It may be specified anywhere on a line.

     The command is usualy "LINK", to specify a normal hypertext link.
     There are other commands as follows:

    @{b}ALINK <path/name> <line>@{ub}
          Display the hypertext node in a new window, starting at <line>.
          This no longer works in V39 and up (why not??).

    @{b}CLOSE@{ub}
          Close the window, to be used with ALINK windows.  Doesn't work in
          V39 and up.

    @{b}LINK <path/name> <line>@{ub}
          Display the hypertext node, starting at <line>.

    @{b}RX <command>@{ub}
          Execute an ARexx script.

    @{b}RXS <command>@{ub}
          Execute an ARexx string file.

    @{b}SYSTEM <command>@{ub}
          Execute an AmigaDOS command.

    @{b}QUIT@{ub}
          Shutdown the current database.  Doesn't work in V39 and up.

     The node is the name of a node in this document, or a path to a node in
     another document.  In AmigaOS 3.0 and above, the node may be a path to
     any type of file which is recognized by the Datatypes system.

@{b}{AMIGAGUIDE}@{ub}
     Displays the word "Amigaguide(R)" in bold.  V40.

@{b}{APEN <n>}@{ub}
     Change the foreground (text) colour to specified pen number.  V40.

@{b}{B}@{ub}
     Turn on bolding.  V39.

@{b}{BG <colour>}@{ub}
     Change the background to a preferences defined colour.  Colour can be:

    @{b}Text@{ub}
    @{b}Shine@{ub}
    @{b}Shadow@{ub}
    @{b}Fill@{ub}
    @{b}FillText@{ub}
    @{b}Background@{ub}
    @{b}Highlight@{ub}
@{b}{BODY}@{ub}
     Restore default formatting for normal body text.  V40.

@{b}{BPEN <n>}@{ub}
     Change the background colour to specified pen number.  V40.

@{b}{CLEARTABS}@{ub}
     Restore default tabs.  V40.

@{b}{CODE}@{ub}
     Turn off wordwrapping.  V40.

@{b}{FG <colour>}@{ub}
     Change the foreground colour.  See command BG for colours.  V40.

@{b}{I}@{ub}
     Turn on italics.  V39.

@{b}{JCENTER}@{ub}
     Turn on centered justification.  V40.

@{b}{JLEFT}@{ub}
     Turn on left justification.  V40.

@{b}{JRIGHT}@{ub}
     Turn on right justification.  V40.

@{b}{LINDENT <n>}@{ub}
     Specify an indent in spaces for the body of paragraphs.  V40.

@{b}{LINE}@{ub}
     Force a line feed without starting a new paragraph.  V40.

@{b}{PAR}@{ub}
     Specifies the end of the paragraph, equivalent to two line feeds and
     usable with SMARTWRAP.  V40.

@{b}{PARD}@{ub}
     Reset to default paragraph settings:  APEN to 1, BPEN to 0, original
     font, and LINDENT to 0.  V40.

@{b}{PARI <n>}@{ub}
     Specify an indent in spaces for the first line of a paragraph.  Value
     is relative to LINDENT and may be negative.  V40.

@{b}{PLAIN}@{ub}
     Turns off all style attributes (bold, italics, underlineing).  V40.

@{b}{SETTABS <n> ... <n>}@{ub}
     Specify a series of tab stops in spaces.  V40.

@{b}{TAB}@{ub}
     Outputs a real tab character.  V40.

@{b}{U}@{ub}
     Turn on underlining.  V39.

@{b}{UB}@{ub}
     Turn off bolding.  V39.

@{b}{UI}@{ub}
     Turn off italics.  V39.

@{b}{UU}@{ub}
     Turn off underlining.  V39.
https://www.lysator.liu.se/amiga/code/guide/amigaguide.guide
*/
         // now need to write  title name in text also
         ofs << "\n  @{b}" << p->_tit << "@{ub}\n\n" ;

        nodeison = true;
    }
    // treat body
    bool firstdot=false;
    for(string s : p->_t)
    {
        if(s.length() ==0) { ofs << "\n";
         continue;
        }
        int nbindent=1;
        while(nbindent<s.length() && (s[nbindent] == ' ' || s[nbindent] == '\t' ) ) nbindent++;
           string sindent;
        for(int i=0;i<nbindent ; i++) sindent +=" ";
        string ttrim = trim(s);
        if(ttrim.length() ==0) { ofs << "\n";
         continue;
        }
        if(ttrim == "***")
        {
            ofs << "\n- - - - - - - - - - - - - - - - - - - - -\n";
            continue;
        }
        ttrim = replaceLinks(ttrim);
        ttrim = replaceBolds(ttrim);
        if(startWith(ttrim,"* ") || startWith(ttrim,"- "))
        {
            // dot
            if(dotindex==0) ofs << "\n";
            dotindex++;

            string otherindex = sindent;
            char dotch = (char)149;
        //    if(startWith(ttrim,"* ") ) otherindex +=
             if(startWith(ttrim,"- "))  {
              if(firstdot) ofs << "@LINDENT "<< (sindent.length()+2);
                otherindex += "   ";
                dotch = '-';
             }else
             {
                if(firstdot) ofs << "@LINDENT "<< (sindent.length());

             }
            // 149 = dot in ansi
            ofs << otherindex << dotch << ttrim.substr(1) << "\n";

            firstdot = false;
            continue;
        } else {
        firstdot = true;
        }
       // [Prerequesite and installation](#prerequesite-and-installation])
       // replace links
      //  if(ttrim)

        vector<string> words = splitt(ttrim," ");
//        if(words.size()>0 && (words[0]=="*" || words[0]=="-"))
//        {
//            // this is a dot

//        }


        bool startline=true;
        int nbwline = 0;
        int ic = 0;
        const int maxcol=68;
        size_t iw;
        for(iw=0; iw<words.size() ; iw++ )
        {
            string w = words[iw];
            if(startline)
            {
                ofs << sindent;
                ic+=sindent.length();
                startline = false;
            }
            ofs << w << ((iw<words.size()-1)?" ":"");
            ic += w.length()+1;

            if(ic>=maxcol)
            {
                ic=0;
                startline = true;
                ofs << "\n";
            }
            nbwline++;
        }
         ofs << "\n";
    dotindex = 0;

//        ttrim, rreplace (ttrim,"**", "");
//        if(ttrim[0]=='-' || (ttrim[0]=='*')



    }

 }
 if(nodeison) ofs << "\n@ENDNODE\n";
/*
  @DATABASE
  @NODE MAIN "Intuition Table of Contents"
  @{"Introduction" LINK Intro}
  @{"Screens" LINK Screen}
  @{"Windows" LINK Window}
  @{"Gadgets" LINK Gadget}
  @{"Menus" LINK Menu}
  @ENDNODE
*/

}

int main(int argc, char **argv)
{
//    if(argc <2) return 0;


      //  curprg = make_shared<paragr>();

    Agdoc d;
    string ppth(/*argv[1]*/"MAME106-MiniMix-1.x-User-Documentation.md");
    d.treatmdfile(ppth);

    ofstream ofsag("Mame106Minimix.guide");
    if(ofsag.good()) d.export_ag(ofsag);

    return 0;
}
