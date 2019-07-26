
void spectrum_frame()
{
   if (!temp.inputblock || input.keymode != K_INPUT::KM_DEFAULT)
      input.make_matrix();

   init_snd_frame();
   init_frame();

   (dbgchk) ? z80dbg::z80loop() : z80fast::z80loop();
   if (modem.open_port) modem.io();

   flush_snd_frame();
   flush_frame();
   showleds();

   if (!cpu.iff1 || // int disabled in CPU
        (conf.mem_model == MM_ATM710 && !(comp.pFF77 & 0x20))) // int disabled by ATM hardware
   {
      unsigned char *mp = am_r(cpu.pc);
      if (cpu.halted) strcpy(statusline, "CPU HALTED"), statcnt = 10;
      if (*(unsigned short*)mp == WORD2(0x18,0xFE) ||
          ((*mp == 0xC3) && *(unsigned short*)(mp+1) == (unsigned short)cpu.pc))
         strcpy(statusline, "CPU STOPPED"), statcnt = 10;
   }

   comp.t_states += conf.frame;
   cpu.t -= conf.frame;
   cpu.eipos -= conf.frame;
   comp.frame_counter++;
}

//Alone Coder 0.37.1C
void do_mem_autoload()
{

	char tmp[0x200]="c:\autoload.bin";
	FILE *ff = fopen(/*tmp*/temp.mem_autoload_filename, "rb");
	unsigned short mem_autoload_shift = 0xffff;
	unsigned char mem_autoload_page = 0;
	fread(&mem_autoload_page, 1, 1, ff);
	fread(&mem_autoload_shift, 1, 2, ff);
	temp.mem_autoload_addr = mem_autoload_page*PAGE + mem_autoload_shift;
	if (mem_autoload_shift<0x4000) fread((RAM_BASE_M+temp.mem_autoload_addr), 1, 64*PAGE, ff);
	fclose(ff);
}

//Alone Coder 0.37.1CFIX
void do_mem_autosave()
{

	char tmp[0x200]="c:\autoload.bin";
	FILE *ff = fopen(/*tmp*/temp.mem_autosave_filename, "wb");
	unsigned char mem_autosave_page = 0;
	unsigned short mem_autosave_shift = 0xffff;
	unsigned int mem_autosave_addr = 0;
	unsigned int mem_autosave_size = 0;
	mem_autosave_page = temp.mem_autosave_addr>>16;
	mem_autosave_shift = temp.mem_autosave_addr & 0x3fff;
	mem_autosave_addr = mem_autosave_page*PAGE + mem_autosave_shift;
    mem_autosave_size = (RAM_BASE_M+mem_autosave_addr)[0]+((RAM_BASE_M+mem_autosave_addr)[1]<<8);
	if ((mem_autosave_addr+2+mem_autosave_size)<=(64*PAGE))
		fwrite((RAM_BASE_M+mem_autosave_addr+2), 1, mem_autosave_size, ff);
	fclose(ff);
}

// version before frame resampler
//uncommented by Alone Coder
void mainloop()
{
   unsigned char skipped = 0;
   for (;;)
   {
      if (skipped < temp.frameskip) skipped++, temp.vidblock = 1;
      else skipped = temp.vidblock = 0;
      temp.sndblock = !conf.sound.enabled;
      temp.inputblock = 0; //temp.vidblock; //Alone Coder
      spectrum_frame();

	if(temp.mem_autoload) do_mem_autoload(); //Alone Coder 0.37.1C
	if(temp.mem_autosave) { //Alone Coder 0.37.1CFIX
	  do_mem_autosave();
	  if (cpu.halted && !cpu.iff1) break;
	}

      // message handling before flip (they paint to rbuf)
      if (!temp.inputblock) dispatch(conf.atm.xt_kbd? ac_main_xt : ac_main);
      if (!temp.vidblock) flip();
      if (!temp.sndblock) do_sound();
   }
}

/*
void __declspec(noreturn) mainloop()
{
   unsigned char skipped = 0;
   for (;;)
   {
      if (skipped < temp.frameskip) skipped++, temp.vidblock = 1;
      else skipped = temp.vidblock = 0;

      if (!temp.vidblock) flip();
      for (unsigned f = rsm.needframes[rsm.frame]; f; f--) {
         temp.sndblock = !conf.sound.enabled;
         temp.inputblock = temp.vidblock;
         spectrum_frame();
         if (!temp.inputblock) dispatch(conf.atm.xt_kbd? ac_main_xt : ac_main);
         if (!temp.sndblock) do_sound();
         if (rsm.mix_frames > 1) {
            memcpy(rbuf_s + rsm.rbuf_dst * rb2_offs, rbuf, temp.scx*temp.scy/4);
            if (++rsm.rbuf_dst == rsm.mix_frames) rsm.rbuf_dst = 0;
         }
      }

      if (++rsm.frame == rsm.period) rsm.frame = 0;
   }
}
*/
