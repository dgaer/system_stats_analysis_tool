// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- // 
// C++ Source Code File
// C++ Compiler Used: GNU
// Produced By: Douglas.Gaer@noaa.gov
// File Creation Date: 10/01/2018
// Date Last Modified: 10/05/2018
// ----------------------------------------------------------- // 
// ------------- Program Description and Details ------------- // 
// ----------------------------------------------------------- // 
/*
This software and documentation is distributed for the purpose of
gathering system statistics. This software can be freely modified and
distributed, but without a warranty of any kind. Use for any purpose
is not guaranteed. All third party libraries used to build this
application are subject to the licensing agreement stated within the
source code and any documentation supplied with the third party
library.

Program used to read CPU stats from a SAR report.
*/
// ----------------------------------------------------------- // 
#include "gxdlcode.h"

#if defined (__USE_ANSI_CPP__) // Use the ANSI Standard C++ library
#include <iostream>
using namespace std; // Use unqualified names for Standard C++ library
#else // Use the old iostream library by default
#include <iostream.h>
#endif // __USE_ANSI_CPP__

#include "stat_collect.h"
#include "m_thread.h"
#include "m_log.h"

struct CPU_stats {
  gxString hour, minute, second, AMPM;
  gxString year, month, day;
  gxString cpu_number;
  gxString user;   // Time that the processor is spending on user processes including time running virtual processors 
  gxString usr;    // Time that the processor is spending on user processes excluding time running virtual processors 
  gxString nice;   // Time spend by CPU for process whose nice value has been changed but a user
  gxString sys;    // Time spend by the processor for operating system tasks excluding hardware and software irqs
  gxString system; // Time spend by the processor for operating system tasks including hardware and software irqs
  gxString iowait; // CPU idle time during which the system had an outstanding disk I/O request
  gxString steal;  // Time spent in involuntary wait by virtual CPU while hypervisor servicing other VMs 
  gxString irq;    // Time spent by the CPU to service hardware interrupts
  gxString soft;   // Time spent by the CPU to service software interrupts
  gxString guest;  // Time spent by the CPU to run a virtual processor
  gxString gnice;  // Time spent by the CPU to run a niced guest
  gxString idle;   // CPU idle time where the system did not have an outstanding disk I/O request
};

// Version number and program name
const char *VersionString = "1.01";
const char *ProgramDescription = "Read CPU stats from SAR";
const char *ProducedBy = "Douglas.Gaer@noaa.gov";
gxString ServiceName = "read_cpu_stats";
gxString ProgramName = ServiceName; // Default program name

// Set the global veriables here
int num_command_line_args = 0;

// Function declarations
void VersionMessage();
void HelpMessage(int short_help_menu = 0);
int ProcessArgs(int argc, char *argv[]);
int ProcessDashDashArg(gxString &arg);

// Start our program's main thread
int main(int argc, char **argv)
{
  int narg;
  char *arg = argv[narg = 1];
  gxString sbuf, fname;
  int num_strings;

  // Load all the command line arguments
  while(narg < argc) {
    if(arg[0] != '\0') {
      if(arg[0] == '-') { // Look for command line arguments
	// Exit if argument is not valid or argument signals program to exit
	if(!ProcessArgs(argc, argv)) {
	  return 1; 
	}
      }
      else {
	// NOTE: Process all non -- or - arguments here
	if(num_strings == 0) fname = arg;
	num_strings++;
      }
    }
    arg = argv[++narg];
  }


  if(num_strings < 1) {
    ConsoleWrite("ERROR - You must input a SAR file name");
    VersionMessage(); // Display the program version information
    HelpMessage(1); // Display the short help menu
    return 1;
  }

  char lbuf[8192]; // Allow really long lines
  DiskFileB ifile, ofile, tfile, mfile;

  if(logcfg->verbose == 1) {
    ConsoleWrite("Verbose mode is on");
  }
  if(logcfg->debug == 1) {
    sbuf << clear <<  logcfg->debug_level;
    ConsoleWrite("Debug mode is on");
    ConsoleWrite("debug_level =", sbuf.c_str());
  }

  ConsoleWrite("Opening SAR file", fname.c_str());
  ifile.df_Open(fname.c_str());
  if(!ifile) {
    ConsoleWrite("ERROR - Cannot open SAR file", fname.c_str());
    if(logcfg->debug == 1) ConsoleWrite(ifile.df_ExceptionMessage());
    return 1;
  }
    
  UString info_line;
  int error_level = 0;
  int has_header = 0;
  int start_section = 0;
  int has_cpu_section = 0;
  gxString *vals1, *vals2, *date_vals, *time_vals, *cpu_header, *cpu_line;
  gxString delimiter1 = " ";
  gxString delimiter2 = "\t";
  gxString date_delimiter = "-";
  gxString time_delimiter = ":";
  unsigned num_arr = 0;
  unsigned i = 0;
  gxString os, kernel, hostname, date, arch, cpu;
  gxString year, month, day;
  gxString time, AMPM, hour,  minute, second;
  int num_cpu_fields = 0;
  int num_cpu_fields_found = 0;
  int AMPM_pos = -1;
  int cpu_number_pos = -1;
  int user_pos = -1;
  int usr_pos = -1;
  int nice_pos = -1;
  int system_pos = -1;
  int sys_pos = -1;
  int iowait_pos = -1;
  int steal_pos = -1;
  int irq_pos = -1;
  int soft_pos = -1;
  int guest_pos = -1;
  int gnice_pos = -1;
  int idle_pos = -1;
  FAU_t file_pos = -1;

  vals1 = vals2 = date_vals = time_vals = cpu_header = cpu_line = 0;

  while(!ifile.df_EOF()) {
    if(error_level > 0) break;
    ifile.df_GetLine(lbuf, sizeof(lbuf), '\n');
    if(ifile.df_GetError() != DiskFileB::df_NO_ERROR) {
      ConsoleWrite("ERROR - Fatal I/O error reading SAR file", fname.c_str());
      if(logcfg->debug == 1) ConsoleWrite(ifile.df_ExceptionMessage());
      error_level = 1;
      break;
    }

    info_line = lbuf;
    info_line.FilterChar('\n');
    info_line.FilterChar('\r');
    info_line.TrimLeadingSpaces();
    info_line.TrimTrailingSpaces();

    if(info_line.is_null()) continue;
    
    // Skip remark lines
    if(info_line[0] == '#') continue; 
    if(info_line[0] == ';') continue; 
    
    // Section start examples for text input files:
    // Linux 2.6.18-426.el5 (rhel5.example.com) <tab>   2018-10-01  
    // Linux 2.6.32-696.28.1.el6.x86_64 (rhel6.example.com) <tab>	10/01/18 	_x86_64_	(4 CPU)
    // Linux 3.10.0-862.11.6.el7.x86_64 (rhel7.example.com) <tab>	2018-09-30 	_x86_64_	(6 CPU)
    num_arr = i = 0;
    vals1 = vals2 = date_vals = time_vals = 0;
    vals1 = ParseStrings(info_line, delimiter1, num_arr);
    if(vals1[0] == "Linux") {
      has_header++;
      if(num_arr < 4) {
	ConsoleWriteLines("ERROR - Bad number of strings line", info_line.c_str());
	error_level = 1;
	break;
      }
      os = vals1[0];
      kernel = vals1[1];
      hostname = vals1[2];
      num_arr = 0;

      vals2 =  ParseStrings(vals1[3], delimiter2, num_arr);
      if(num_arr < 2) { 
	ConsoleWriteLines("ERROR - Bad date line", vals1[3].c_str());
	error_level = 1;
	break;
      }

      date = vals2[1];
      if(num_arr >= 3) arch = vals2[2];
      if(num_arr >= 4) cpu = vals2[3];
      num_arr = 0;
      if(date.length() == 10) {
	// 4 digit year
	if(date.Find("-") != -1) {
	  // YYYY-MM-DD
	  date_delimiter = "-";
	  date_vals =  ParseStrings(date, date_delimiter, num_arr);
	  if(num_arr != 3) { 
	    ConsoleWriteLines("ERROR - Bad date value", date.c_str());
	    error_level = 1;
	    break;
	  }
	  year = date_vals[0];
	  month = date_vals[1];
	  day = date_vals[2];
	}
	else if(date.Find("/") != -1) {
	  // MM/DD/YYYY
	  date_delimiter = "/";
	  date_vals =  ParseStrings(date, date_delimiter, num_arr);
	  if(num_arr != 3) { 
	    ConsoleWriteLines("ERROR - Bad date value", date.c_str());
	    error_level = 1;
	    break;
	  }
	  month = date_vals[0];
	  day = date_vals[1];
	  year = date_vals[2];
	}
	else {
	  ConsoleWriteLines("ERROR - Bad date string", date.c_str());
	  error_level = 1;
	  break;
	}
      }
      else {
	// 2 digit year
	if(date.Find("-") != -1) {
	  // YY-MM-DD
	  date_delimiter = "-";
	  date_vals =  ParseStrings(date, date_delimiter, num_arr);
	  if(num_arr != 3) { 
	    ConsoleWriteLines("ERROR - Bad date value", date.c_str());
	    error_level = 1;
	    break;
	  }
	  year << clear << "20" << date_vals[0];
	  month = date_vals[1];
	  day = date_vals[2];
	}
	else if(date.Find("/") != -1) {
	  // MM/DD/YY
	  date_delimiter = "/";
	  date_vals =  ParseStrings(date, date_delimiter, num_arr);
	  if(num_arr != 3) { 
	    ConsoleWriteLines("ERROR - Bad date value", date.c_str());
	    error_level = 1;
	    break;
	  }
	  month = date_vals[0];
	  day = date_vals[1];
	  year << clear << "20" << date_vals[2];
	}
	else {
	  ConsoleWriteLines("ERROR - Bad date string", date.c_str());
	  error_level = 1;
	  break;
	}
      }
      hostname.FilterChar('('); hostname.FilterChar(')');
      sbuf << clear << hostname << " " << "kernel:" << kernel << " " << month << "/" << day << "/" << year;
      ConsoleWrite(sbuf.c_str());
      start_section = 1;
      ifile.df_GetLine(lbuf, sizeof(lbuf), '\n'); // Read the next blank line
    }
    
    if(info_line.Find(" CPU ") != -1) {
      start_section = 1;
      has_cpu_section = 1;
    }

    if(vals1 != 0) { delete[] vals1; vals1 = 0; }
    if(vals2 != 0) { delete[] vals2; vals2 = 0; }
    if(date_vals != 0) { delete[] date_vals; date_vals = 0; }

    if(start_section) {
      while(!ifile.df_EOF()) {
	if(error_level > 0) break;
	if(has_cpu_section == 0) {
	  ifile.df_GetLine(lbuf, sizeof(lbuf), '\n');
	  if(ifile.df_GetError() != DiskFileB::df_NO_ERROR) {
	    ConsoleWrite("ERROR - Fatal I/O error reading SAR file", fname.c_str());
	    if(logcfg->debug == 1) ConsoleWrite(ifile.df_ExceptionMessage());
	    error_level = 1;
	    break;
	  }
	} 
	else {
	  has_cpu_section = 0;
	}

	info_line = lbuf;
	info_line.FilterChar('\n');
	info_line.FilterChar('\r');
	info_line.TrimLeadingSpaces();
	info_line.TrimTrailingSpaces();
	if(info_line.is_null()) { break; } // end of section
	
	// Skip remark lines
	if(info_line[0] == '#') continue; 
	if(info_line[0] == ';') continue; 

	if(info_line.Find(" CPU ") != -1) {
	  AMPM_pos = -1;
	  cpu_number_pos = -1;
	  user_pos = -1;
	  usr_pos = -1;
	  nice_pos = -1;
	  system_pos = -1;
	  sys_pos = -1;
	  iowait_pos = -1;
	  steal_pos = -1;
	  irq_pos = -1;
	  soft_pos = -1;
	  guest_pos = -1;
	  gnice_pos = -1;
	  idle_pos = -1;
	  num_cpu_fields_found = 0;

	  // Replace multiple spaces in between values
	  while(info_line.IFind("  ") != -1) info_line.ReplaceString("  ", " ");
	  info_line.FilterChar('%');
	  num_arr = 0;
	  cpu_header = ParseStrings(info_line, delimiter1, num_arr);
	  for(i = 0; i < num_arr; i++) {
	    if(i == 0) time = cpu_header[0];
	    if(cpu_header[i] == "AM" || cpu_header[i] == "PM") AMPM_pos = i;
	    if(cpu_header[i] == "CPU") cpu_number_pos = i;
	    if(cpu_header[i] == "user") {  user_pos = i; num_cpu_fields_found++; }
	    if(cpu_header[i] == "usr") { usr_pos = i; num_cpu_fields_found++; }
	    if(cpu_header[i] == "nice") { nice_pos = i; num_cpu_fields_found++; }
	    if(cpu_header[i] == "system") { system_pos = i; num_cpu_fields_found++; }
	    if(cpu_header[i] == "sys") { sys_pos = i; num_cpu_fields_found++; }
	    if(cpu_header[i] == "iowait") { iowait_pos = i; num_cpu_fields_found++; }
	    if(cpu_header[i] == "steal") { steal_pos = i; num_cpu_fields_found++; }
	    if(cpu_header[i] == "irq") { irq_pos = i; num_cpu_fields_found++; }
	    if(cpu_header[i] == "soft") { soft_pos = i; num_cpu_fields_found++; }
	    if(cpu_header[i] == "guest") { guest_pos = i; num_cpu_fields_found++; }
	    if(cpu_header[i] == "gnice") { gnice_pos = i; num_cpu_fields_found++; }
	    if(cpu_header[i] == "idle") { idle_pos = i; num_cpu_fields_found++; }
	  }
	  if(cpu_header != 0) { delete[] cpu_header; cpu_header = 0; }
	  num_cpu_fields = num_arr;

	  if(num_cpu_fields_found == 0) {
	    break; // Have CPU section but unknown fields
	  }

	  // Read in all the CPU lines
	  while(!ifile.df_EOF()) {
	    if(error_level > 0) break;
	    ifile.df_GetLine(lbuf, sizeof(lbuf), '\n');
	    if(ifile.df_GetError() != DiskFileB::df_NO_ERROR) {
	      ConsoleWrite("ERROR - Fatal I/O error reading SAR file", fname.c_str());
	      if(logcfg->debug == 1) ConsoleWrite(ifile.df_ExceptionMessage());
	      error_level = 1;
	      break;
	    }
	    info_line = lbuf;
	    info_line.FilterChar('\n');
	    info_line.FilterChar('\r');
	    info_line.TrimLeadingSpaces();
	    info_line.TrimTrailingSpaces();
	    if(info_line.is_null()) { break; } // End of section
	    
	    // Skip remark lines
	    if(info_line[0] == '#') continue; 
	    if(info_line[0] == ';') continue;

	    while(info_line.IFind("  ") != -1) info_line.ReplaceString("  ", " ");
	    cpu_line = ParseStrings(info_line, delimiter1, num_arr);
	    if(num_arr != num_cpu_fields) {
	      if(num_arr > 0) {
		if(cpu_line[0] == "Linux") {
		  file_pos = ifile.df_Tell();
		  ifile.df_Seek(file_pos);
		  break; // End of section  
		}
		if(cpu_line[0] == "Average:") { // We will calculate our own averages
		  delete[] cpu_line; cpu_line = 0;
		  continue;  
		}
	      }
	      sbuf << clear << "CPU header line has " << num_arr << " fields " << " line has " << num_cpu_fields << " fields";
	      ConsoleWriteLines("ERROR - Bad number of strings parsing CPU line", sbuf.c_str(), info_line.c_str());
	      error_level = 1;
	      break;
	    }

	    if(cpu_line[0] == "Average:") { // We will calculate our own averages
	      delete[] cpu_line; cpu_line = 0;
	      continue;  
	    }

	    num_arr = 0;
	    time_vals = 0;
	    time_vals =  ParseStrings(cpu_line[0], time_delimiter, num_arr);
	    if(num_arr != 3) { 
	      ConsoleWriteLines("ERROR - Bad time value", date.c_str());
	      error_level = 1;
	      if(time_vals) { delete[] time_vals; time_vals = 0; }
	      break;
	    }
	    hour = time_vals[0];
	    minute = time_vals[1];
	    second = time_vals[2];
	    delete[] time_vals; time_vals = 0;

	    sbuf << clear << month << "/" << day << "/" << year << " " << hour << ":" << minute << ":" << second;
	    if(AMPM_pos > 0) sbuf << " " << cpu_line[AMPM_pos].c_str();
	    cout << "time = " << sbuf.c_str() << "\n";

	    if(cpu_number_pos > 0) cout << "cpu_number = " << cpu_line[cpu_number_pos].c_str() << "\n";
	    if(user_pos > 0) cout << "user = " << cpu_line[user_pos].c_str() << "\n";
	    if(usr_pos > 0) cout << "usr = " << cpu_line[usr_pos].c_str() << "\n";
	    if(nice_pos > 0) cout << "nice = " << cpu_line[nice_pos].c_str() << "\n";
	    if(system_pos > 0) cout << "system = " << cpu_line[system_pos].c_str() << "\n";
	    if(sys_pos > 0) cout << "sys = " << cpu_line[sys_pos].c_str() << "\n";
	    if(iowait_pos > 0) cout << "iowait = " << cpu_line[iowait_pos].c_str() << "\n";
	    if(steal_pos > 0) cout << "steal = " << cpu_line[steal_pos].c_str() << "\n";
	    if(irq_pos > 0) cout << "irq = " << cpu_line[irq_pos].c_str() << "\n";
	    if(soft_pos > 0) cout << "soft = " << cpu_line[soft_pos].c_str() << "\n";
	    if(guest_pos > 0) cout << "guest = " << cpu_line[guest_pos].c_str() << "\n";
	    if(gnice_pos > 0) cout << "gnice = " << cpu_line[gnice_pos].c_str() << "\n";
	    if(idle_pos > 0) cout << "idle = " << cpu_line[idle_pos].c_str() << "\n";

	    if(cpu_line) { delete[] cpu_line; cpu_line = 0; }
	  }
	  if(cpu_line) { delete[] cpu_line; cpu_line = 0; }
	}
      }
      if(cpu_header != 0) { delete[] cpu_header; cpu_header = 0; }
    }
    start_section = 0; 

  }

  // Free any memory that might not have been deleted during an error condition
  if(vals1 != 0) { delete[] vals1; vals1 = 0; }
  if(vals2 != 0) { delete[] vals2; vals2 = 0; }
  if(date_vals != 0) { delete[] date_vals; date_vals = 0; }
  if(time_vals != 0) { delete[] time_vals; time_vals = 0; }
  if(cpu_header != 0) { delete[] cpu_header; cpu_header = 0; }
  if(cpu_line) { delete[] cpu_line; cpu_line = 0; }

  if(has_header == 0) {
    ConsoleWrite("ERROR - No SAR information found in file", fname.c_str());
    error_level = 1;
  }

  return error_level;
}

void VersionMessage()
{
  GXSTD::cout << "\n" << GXSTD::flush;
  GXSTD::cout << ProgramDescription << " version " << VersionString  << "\n" << GXSTD::flush;
  GXSTD::cout << "Produced by: " << ProducedBy << "\n" << GXSTD::flush;
  GXSTD::cout << "\n" << GXSTD::flush;
}

void HelpMessage(int short_help_menu)
{
  if(short_help_menu) {
    if(num_command_line_args > 0) {
      GXSTD::cout << "Starting program with command line arguments." << "\n" << GXSTD::flush;
    }
    else {
      GXSTD::cout << "Starting program using default arguments." << "\n" << GXSTD::flush;
    }
    GXSTD::cout << "Run with the --help argument for list of switches." << "\n" << GXSTD::flush;
    GXSTD::cout << "\n" << GXSTD::flush;
    return;
  }
  
  // We are displaying the long help menu
  GXSTD::cout << "Usage: " << ProgramName << " [args]" << "\n" << GXSTD::flush;
  GXSTD::cout << "args: " << "\n" << GXSTD::flush;
  GXSTD::cout << "     --help                Print help message and exit" << "\n" << GXSTD::flush;
  GXSTD::cout << "     --version             Print version number and exit" << "\n" << GXSTD::flush;
  GXSTD::cout << "     --debug               Enable debug mode default level --debug=1" << "\n" << GXSTD::flush;
  GXSTD::cout << "     --verbose             Turn on verbose messaging to stderr" << "\n" << GXSTD::flush;
  GXSTD::cout << "\n" << GXSTD::flush;
  return;
}

int ProcessArgs(int argc, char *argv[])
{
  // process the program's argument list
  int i;
  gxString sbuf;
  num_command_line_args = 0;

  for(i = 1; i < argc; i++ ) {
    if(*argv[i] == '-') {
      char sw = *(argv[i] +1);
      switch(sw) {
	case '?':
	  HelpMessage();
	  return 0; // Signal program to exit

	case '-':
	  sbuf = &argv[i][2]; 
	  // Add all -- prepend filters here
	  sbuf.TrimLeading('-');
	  if(!ProcessDashDashArg(sbuf)) return 0;
	  break;

	default:
	  GXSTD::cout << "Error unknown command line switch " << argv[i] << "\n" << flush;
	  return 0;
      }
      num_command_line_args++;
    }
  }
  
  return 1; // All command line arguments were valid
}

int ProcessDashDashArg(gxString &arg)
{
  gxString sbuf, equal_arg;
  int has_valid_args = 0;
  
  if(arg.Find("=") != -1) {
    // Look for equal arguments
    // --log-file="/var/log/my_service.log"
    equal_arg = arg;
    equal_arg.DeleteBeforeIncluding("=");
    arg.DeleteAfterIncluding("=");
    equal_arg.TrimLeading(' '); equal_arg.TrimTrailing(' ');
    equal_arg.TrimLeading('\"'); equal_arg.TrimTrailing('\"');
    equal_arg.TrimLeading('\''); equal_arg.TrimTrailing('\'');
  }

  arg.ToLower();

  // Process all -- arguments here
  if(arg == "help") {
    HelpMessage();
    return 0; // Signal program to exit
  }
  if(arg == "?") {
    HelpMessage();
    return 0; // Signal program to exit
  }

  if((arg == "version") || (arg == "ver")) {
    VersionMessage();
    return 0; // Signal program to exit
  }

  if(arg == "debug") {
    logcfg->debug = 1;
    if(!equal_arg.is_null()) { 
      logcfg->debug_level = equal_arg.Atoi(); 
    }
    has_valid_args = 1;
  }
  if(arg == "verbose") {
    logcfg->verbose = 1;
    has_valid_args = 1;
  }

  if(!has_valid_args) {
    GXSTD::cout << "Error unknown command line switch --" << arg << "\n" << flush;
  }
  arg.Clear();
  return has_valid_args;
}
// ----------------------------------------------------------- // 
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
