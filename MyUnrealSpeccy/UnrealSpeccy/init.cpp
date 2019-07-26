
void cpu_info()
{
   char idstr[64]; fillCpuString(idstr); trim(idstr);
   unsigned cpuver = cpuid(1,0);
   unsigned features = cpuid(1,1);
   temp.mmx = (features >> 23) & 1;
   temp.sse2 = (features >> 26) & 1;
   temp.cpufq = GetCPUFrequency();

   color(CONSCLR_HARDITEM); printf("cpu: ");

   color(CONSCLR_HARDINFO);
   printf("%s ", idstr);

   color(CONSCLR_HARDITEM);
   printf("%d.%d.%d [MMX:%s,SSE2:%s] ",
      (cpuver>>8) & 0x0F, (cpuver>>4) & 0x0F, cpuver & 0x0F,
      temp.mmx ? "YES" : "NO",
      temp.sse2 ? "YES" : "NO");

   color(CONSCLR_HARDINFO);
   printf("at %d MHz\n", (unsigned)(temp.cpufq/1000000));

#ifdef MOD_SSE2
   if (!temp.sse2) {
      color(CONSCLR_WARNING);
      printf("warning: this is an SSE2 build, recompile or download non-P4 version\n");
   }
#else MOD_SSE2
   if (temp.sse2) {
      color(CONSCLR_WARNING);
      printf("warning: SSE2 disabled in compile-time, recompile or download P4 version\n");
   }
#endif
}

void restrict_version(char legacy)
{
//   color(CONSCLR_WARNING);
//   printf ("WARNING: Windows 95/98/Me is not fully supported.\n");
   temp.win9x=1; //Dexus
   color();

/*   static const char vererror[] = "unsupported OS version";

   if (!legacy) errexit(vererror);

   #ifdef MOD_9X
   static const char verwarning[] =
      "detected windows version is not supported\r\n"
      "by current version of UnrealSpeccy and untested.\r\n\r\n"
      "you may click 'YES' to continue on your own risk.\r\n"
      "in this case you will experience crashes under\r\n"
      "some conditions. it's an OS-specific problem, please\r\n"
      "don't report it and consider using NT-based system";
   if (MessageBox(0, verwarning, vererror, MB_ICONERROR | MB_YESNO | MB_DEFBUTTON2) == IDNO) errexit(vererror);
   #endif
*/
}

unsigned int unhexdig(char c)
{
  return((c<='9')?(c-'0'):(10+((c<'a')?(c-'A'):(c-'a'))));
}

unsigned int unhex(char* str)
{
  return(unhexdig(str[0])<<20)|
        (unhexdig(str[1])<<16)|
		(unhexdig(str[2])<<12)|
		(unhexdig(str[3])<< 8)|
		(unhexdig(str[4])<< 4)|
		(unhexdig(str[5])<< 0);
}

void init_all(int argc, char **argv)
{
   cpu_info();

   char *config = 0, legacy = 0;
   for (int i = 0; i < argc; i++) {
      if (argv[i][0] != '/' && argv[i][0] != '-') continue;
      if (!stricmp(argv[i]+1, "i") && i+1 < argc)
         config = argv[i+1], i++;
      #ifdef MOD_9X
      if (argv[i][1] == '9') legacy = 1;
      #endif
   }
   temp.win9x=0; //Dexus
   if (GetVersion() >> 31) restrict_version(legacy);

   init_z80tables();
   video_permanent_tables();
   init_ie_help();
   load_config(config);
   //make_samples();
   #ifdef MOD_GS
   init_gs();
   #endif
   init_leds();
   init_tape();
   init_hdd_cd();
   start_dx();

//Alone Coder 0.37.1C -----
   temp.mem_autoload=0;
   for (auto i = 0; i < argc; i++) {
      if (argv[i][0] != '/' && argv[i][0] != '-') continue;
      if (!stricmp(argv[i]+1, "c") && (i+1 < argc))
	  {
		 temp.mem_autoload=1;
         //temp.mem_autoload_filename = argv[i+1];
		 GetFullPathName(argv[i+1], sizeof (temp.mem_autoload_filename), temp.mem_autoload_filename, NULL);
		 //MessageBox(dlg, temp.mem_autoload_filename, "OK", MB_ICONERROR | MB_OK);
		 i++;
	  }
   }
//-----

//Alone Coder 0.37.1CFIX -----
   temp.mem_autosave=0;
   for (auto i = 0; i < argc; i++) {
      if (argv[i][0] != '/' && argv[i][0] != '-') continue;
      if ((argv[i][1]=='s') && (i+1 < argc))
	  {
		 temp.mem_autosave=1;
		 temp.mem_autosave_addr=unhex(argv[i]+2);
		 GetFullPathName(argv[i+1], sizeof (temp.mem_autosave_filename), temp.mem_autosave_filename, NULL);
		 printf("auto-saving from %x\n",temp.mem_autosave_addr);
	  }
   }
//-----

   applyconfig();
   main_reset();
   autoload();

   load_errors = 0;
   trd_toload = 0;
   *(DWORD*)trd_loaded = 0; // clear loaded flags, don't see autoload'ed images

   if(argc) do {
      if (**argv == '-' || **argv == '/') {
         //if (argc > 1 && !stricmp(argv[0]+1, "i"))
         if (argc > 1 && (!stricmp(argv[0]+1, "i") || !stricmp(argv[0]+1, "c") || (argv[0][1]=='s'))) //Alone Coder 0.37.1CFIX
		 {
			 //MessageBox(dlg, "a", "OK", MB_ICONERROR | MB_OK);
			 argc--, argv++;
			 argc--, argv++;
		 }
         continue;
      }

      char fname[0x200], *temp;
      GetFullPathName(*argv, sizeof fname, fname, &temp);

      trd_toload = -1; // auto-select
      if (!loadsnap(fname)) errmsg("error loading <%s>", *argv), load_errors = 1;
	  argc--, argv++;
   } while(argc);

   if (load_errors) {
      int code = MessageBox(wnd, "Some files, specified in\r\ncommand line, failed to load\r\n\r\nContinue emulation?", "File loading error", MB_YESNO | MB_ICONWARNING);
      if (code != IDYES) exit();
   }

   SetCurrentDirectory(conf.workdir);
   timeBeginPeriod(1);
}

void __declspec(noreturn) exit()
{
   exitflag = 1;
   if (savesndtype) savesnddialog();
   done_tape();
   done_dx();
   done_leds();
   save_nv();
   modem.close();
   done_ie_help();
   timeEndPeriod(1);
   if (ay[1].Chip2203) YM2203Shutdown(ay[1].Chip2203); //Dexus
   if (ay[0].Chip2203) YM2203Shutdown(ay[0].Chip2203); //Dexus
   color();
   printf("\nsee you later!");
   if (!nowait) SetConsoleTitle("press a key..."), getch();
   ExitProcess(0);
}
