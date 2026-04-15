#include <array>
#include <iostream>
#include <memory>
#include <string>
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <unistd.h>

using namespace std;

// Constantes de couleur
const string CBK = "\033[30m";
const string CRE = "\033[31m";
const string CGR = "\033[32m";
const string CYE = "\033[33m";
const string CBL = "\033[34m";
const string CMA = "\033[35m";
const string CCY = "\033[36m";
const string CWH = "\033[37m";
const string CBD = "\033[1m";
const string CNC = "\033[0m";

string execCmd(const char* cmd) {
    array<char, 128> buffer;
    string result;
    unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);

    if (!pipe) return "";

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }

    return result;
}

string draw(int perc, int size) {
    const string FULL = "━";
    const string EMPTY = "━";
    string out;
    int inc = perc * size / 100;

    for (int v = 0; v < size; v++) {
        out += (v <= inc) ? CMA + FULL : CWH + EMPTY;
    }

    return out;
}

string getColorBar() {
    string c = "\033[0m\033[31m░▒";

    for (int i = 1; i <= 6; i++) {
        c += "\033[" + to_string(i + 41) + "m\033[" + to_string(i + 30) + "m█▓▒░";
    }

    return c + "\033[37m█\033[0m▒░";
}

int main() {
  std::string name = getenv("USER");
  struct utsname uts;
  uname(&uts);
  std::string host = uts.nodename;

  std::string distro =
      execCmd("awk -F '\"' '/PRETTY_NAME/ { print $2 }' /etc/os-release");
  std::string packages;
  if (distro == "Gentoo Linux")
      packages = execCmd("ls /var/db/pkg/* | wc -l");
  else
      packages = execCmd("pacman -Q | wc -l");
  std::string wm =
      execCmd("xprop -id $(xprop -root -notype | awk "
              "'$1==\"_NET_SUPPORTING_WM_CHECK:\"{print $5}') -notype -f "
              "_NET_WM_NAME 8t | grep \"WM_NAME\" | cut -f2 -d \\\"");
  std::string storage =
      execCmd("df -h --output=used / | awk 'NR == 2 { print $1 }'");
  std::string term =
      execCmd("ps -o comm= -p $(ps -o ppid= -p $(ps -o sid= -p $$))");
  std::string uptm =
      execCmd("uptime -p | sed -e 's/hour/hr/' -e 's/hours/hrs/' -e "
              "'s/minutes/mins/' -e 's/minute/min/' -e 's/up //'");

  int c_lvl =
      std::stoi(execCmd("grep 'cpu ' /proc/stat | awk "
                        "'{usage=($2+$4)*100/($2+$4+$5)} END {print usage}'"));
  std::string CPU = CGR + "cpu" + CNC + " " + std::to_string(c_lvl) + "% " +
                    draw(c_lvl, 14) + "\n";

  int ram_lvl =
      std::stoi(execCmd("free | awk '/Mem:/ {print int($3/$2 * 100.0)}'"));
  std::string RAM = CYE + "ram" + CNC + " " + std::to_string(ram_lvl) + "% " +
                    draw(ram_lvl, 14) + "\n";

  int disk_lvl = std::stoi(
      execCmd("df -h | grep '/$' | tr -s ' ' | cut -d ' ' -f5 | sed 's/%//'"));
  std::string DISK = CCY + "disk" + CNC + " " + std::to_string(disk_lvl) +
                     "% " + draw(disk_lvl, 14) + "\n";

  std::cout << CBD << "                  " << CBL << ".\n";
  std::cout << "                 " << CBL << "/ " << CMA << "\\\n";
  std::cout << "                " << CBL << "/   " << CMA << "\\\n";
  std::cout << "               " << CBL << "/^.   " << CMA << "\\\n";
  std::cout << "              " << CBL << "/  ." << CWH << "-" << CMA
            << ".  \\\n";
  std::cout << "             " << CBL << "/  (   " << CMA << ") _\\\n";
  std::cout << "            " << CBL << "/ *.~   " << CMA << "~.*^\\\n";
  std::cout << "           " << CBL << "/.^         " << CMA << "^.\\\n";

  std::cout << "\033[0m\n    " << getColorBar() << "\033[0m\n\n";

  std::cout << "             Hi " << CRE << CBD << name << CNC << "\n";
  std::cout << "       Welcome to " << CGR << CBD << host << CNC << "\n";
  std::cout << "           up " << CCY << CBD << uptm << CNC << "\n\n";

  std::cout << CRE << "         distro " << CWH << "⏹ " << CNC << distro
            << "\n";
  std::cout << CBL << "         kernel " << CWH << "⏹ " << CNC << uts.release
            << "\n";
  std::cout << CMA << "       packages " << CWH << "⏹ " << CNC << packages
            << "\n";
  std::cout << CGR << "          shell " << CWH << "⏹ " << CNC
            << execCmd("echo $SHELL | sed 's/.*\\///'") << "\n";
  std::cout << CYE << "           term " << CWH << "⏹ " << CNC << term << "\n";
  std::cout << CCY << "             wm " << CWH << "⏹ " << CNC << wm << "\n";
  std::cout << CBL << "           disk " << CWH << "⏹ " << CNC << storage
            << " used\n";
  std::cout << CNC << "\n";

  std::cout << "        " << CPU;
  std::cout << "        " << RAM;
  std::cout << "       " << DISK;

  return 0;
}
