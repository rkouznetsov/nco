/* $Header: /data/zender/nco_20150216/nco/src/nco/ncap.c,v 1.10 1999-10-04 05:13:34 zender Exp $ */

/* ncap -- netCDF arithmetic processor */

/* (c) Copyright 1995--1999 University Corporation for Atmospheric Research 
   The file LICENSE contains the full copyright notice 
   Contact NSF/UCAR/NCAR/CGD/CMS for copyright assistance */

/* Purpose: Compute user-defined derived fields using forward algebraic notation applied to netCDF files */

/* Usage:
   ncap -O -D 1 -S ncap.in in.nc foo.nc
   ncap -O -D 1 -s a=b+c -s "b=c-d/2." -S ncap.in in.nc foo.nc
   ncap -O -D 1 -S ncap.in in.nc foo.nc
   ncap -O -D 1 -s two=one+two in.nc foo.nc
   */

/* Standard header files */
#include <math.h>               /* sin cos cos sin 3.14159 */
#include <stdio.h>              /* stderr, FILE, NULL, etc. */
#include <stdlib.h>             /* atof, atoi, malloc, getopt */ 
#include <string.h>             /* strcmp. . . */
#include <sys/stat.h>           /* stat() */
#include <time.h>               /* machine time */
#include <unistd.h>             /* all sorts of POSIX stuff */ 

/* #define MAIN_PROGRAM_FILE MUST precede #include nc.h */
#define MAIN_PROGRAM_FILE
#include <netcdf.h>             /* netCDF def'ns */
#include "nc.h"                 /* global definitions */

#ifndef LINUX
#include "getopt.h" /* GNU getopt() */
#else /* LINUX */
#include <getopt.h> /* GNU getopt() is standard on Linux */
#endif /* LINUX */

int 
main(int argc,char **argv)
{
  /* rank reducers: min max stddev */ 
  /* binary math functions: pow atan2 mod */
  /* relational operators: > >= < <= == != */ 
  /* don't return floats: abs */ 
  /* can't find on cray: expm1,lgamma,log1p */ 

  /* Place function names and entry points in the symbol table before processing input tokens */
  extern double acos();
  extern double asin();
  extern double atan();
  extern double cos();
  extern double exp();
  extern double gamma();
  extern double log();
  extern double log10();
  extern double sin();
  extern double sqrt();
  extern double tan();

  extern FILE *yyin;

  bool EXCLUDE_INPUT_LIST=False; /* Option c */ 
  bool FILE_RETRIEVED_FROM_REMOTE_LOCATION;
  bool FORCE_APPEND=False; /* Option A */ 
  bool FORCE_OVERWRITE=False; /* Option O */ 
  bool FORTRAN_STYLE=False; /* Option F */
  bool HISTORY_APPEND=True; /* Option h */
  bool NCAR_CSM_FORMAT;
  bool PROCESS_ALL_COORDINATES=False; /* Option c */
  bool PROCESS_ASSOCIATED_COORDINATES=True; /* Option C */
  bool REMOVE_REMOTE_FILES_AFTER_PROCESSING=True; /* Option R */ 

  char **var_lst_in=NULL_CEWI;
  char **fl_lst_abb=NULL; /* Option n */
  char **fl_lst_in;
  char *fl_spt=NULL; /* Option s */ 
  char *fl_in=NULL;
  char *fl_pth_lcl=NULL; /* Option l */ 
  char *lmt_arg[MAX_NC_DIMS];
  char *spt_arg[73];
  char *opt_sng;
  char *fl_out;
  char *fl_out_tmp;
  char *fl_pth=NULL; /* Option p */ 
  char *time_buf_srt;
  char *cmd_ln;
  char CVS_Id[]="$Id: ncap.c,v 1.10 1999-10-04 05:13:34 zender Exp $"; 
  char CVS_Revision[]="$Revision: 1.10 $";
  
  dim_sct **dim;
  dim_sct **dim_out;
  
  extern char *optarg;
  extern int optind;

  int idx;
  int in_id;  
  int int_foo;
  int out_id;  
  int nbr_abb_arg=0;
  int nbr_dim_fl;
  int nbr_lmt=0; /* Option d. NB: nbr_lmt gets incremented */
  int nbr_spt=0; /* Option s. NB: nbr_spt gets incremented */
  int nbr_var_fl;
  int nbr_var_fix; /* nbr_var_fix gets incremented */ 
  int nbr_var_prc; /* nbr_var_prc gets incremented */ 
  int nbr_xtr=0; /* nbr_xtr won't otherwise be set for -c with no -v */ 
  int nbr_dim_xtr;
  int nbr_fl=0;
  int opt;
  int rec_dim_id=-1;
  int rcd; /* Return code */ 
  
  lmt_sct *lmt=NULL_CEWI;
  
  nm_id_sct *dim_lst;
  nm_id_sct *xtr_lst=NULL; /* xtr_lst can get realloc()'d from NULL with -c option */ 
  
  time_t clock;
  
  var_sct **var;
  var_sct **var_fix;
  var_sct **var_fix_out;
  var_sct **var_out;
  var_sct **var_prc;
  var_sct **var_prc_out;
  
  /* prs_sct must be consistent between ncap.y and ncap.c
   DBG XXX: Is there a way to define prs_sct in only one place? */ 
  typedef struct{
    char *fl_in;
    int in_id;  
    char *fl_out;
    int out_id;  
    char *sng;
    dim_sct **dim;
    int nbr_dim_xtr;
  } prs_sct;
  prs_sct prs_arg;

  /* Start the clock and save the command line */  
  cmd_ln=cmd_ln_sng(argc,argv);
  clock=time((time_t *)NULL);
  time_buf_srt=ctime(&clock);

  /* Get the program name and set the enum for the program (e.g., prg=ncra) */
  prg_nm=prg_prs(argv[0],&prg);

  /* Parse command line arguments */
  while(1){
    int opt_idx=0;

    static struct option opt_long[] =
      {
	/* The option structure is {char *name,int has_arg,int *flag,int val} 
	   has_arg is compared to enum _argtype{no_argument,required_argument,optional_argument}, 
	   flag points to a variable that gets set to val whenever the name option is set.
	   For long options that have a zero flag field, getopt() returns the contents of val.
	 */ 
	{"debug",1,0,'D'},
	{"dbg_lvl",1,0,'D'},
	{"output",1,0,'o'},
	{"overwrite",0,0,'O'},
	{"script",1,0,'S'},
	{"version",0,0,'r'},
	{"verbose",0,0,'D'},
	/* The last option must have a name of "0" to signal to getopt_long() to stop processing */ 
	{0,0,0,0}
      };
    
    opt_sng="ACcD:d:Fhl:n:Op:rs:S:Rv:x";
    opt=getopt_long_only(argc,argv,opt_sng,opt_long,&opt_idx);
    
    /* Is it time to parse the positional arguments yet? */ 
    if(opt == EOF) break;
    
    switch(opt){
    case 0:
      (void)fprintf(stderr,"option %s",opt_long[opt_idx].name);
      if(optarg) (void)fprintf(stderr," with arg %s",optarg);
      (void)fprintf(stderr,"\n");

      if(!strcmp(opt_long[opt_idx].name,"debug")){
	if(optarg) dbg_lvl=(unsigned short int)atoi(optarg);
	 (void)fprintf(stderr,"dbg_lvl = %d\n",dbg_lvl);
       } /* end if */ 
      break;
    case 'A': /* Toggle FORCE_APPEND */
      FORCE_APPEND=!FORCE_APPEND;
      break;
    case 'C': /* Add to the extraction list any coordinates associated with variables to be extracted? */ 
      PROCESS_ASSOCIATED_COORDINATES=False;
      break;
    case 'c':
      PROCESS_ALL_COORDINATES=True;
      break;
    case 'D': /* The debugging level.  Default is 0. */
      dbg_lvl=atoi(optarg);
      break;
    case 'd': /* Copy the argument for later processing */ 
      lmt_arg[nbr_lmt]=(char *)strdup(optarg);
      nbr_lmt++;
      break;
    case 'F': /* Toggle the style of printing out arrays. Default is C-style. */
      FORTRAN_STYLE=!FORTRAN_STYLE;
      break;
    case 'h': /* Toggle appending to history global attribute */
      HISTORY_APPEND=!HISTORY_APPEND;
      break;
    case 'l': /* Get the local path prefix for storing files retrieved from the remote file system */
      fl_pth_lcl=optarg;
      break;
    case 'n': /* Get the NINTAP-style abbreviation of files to average */ 
      fl_lst_abb=lst_prs(optarg,",",&nbr_abb_arg);
      if(nbr_abb_arg < 1 || nbr_abb_arg > 3){
	(void)fprintf(stdout,"%s: ERROR Incorrect abbreviation for file list\n",prg_nm);
	(void)usg_prn();
	exit(EXIT_FAILURE);
      } /* end if */
      break;
    case 'O': /* Toggle FORCE_OVERWRITE */
      FORCE_OVERWRITE=!FORCE_OVERWRITE;
      break;
    case 'p': /* Get the path prefix */
      fl_pth=optarg;
      break;
    case 'R': /* Toggle the removal of remotely-retrieved-files after processing. Default is True */
      REMOVE_REMOTE_FILES_AFTER_PROCESSING=!REMOVE_REMOTE_FILES_AFTER_PROCESSING;
      break;
    case 'r': /* Print the CVS program info and copyright notice */
      (void)copyright_prn(CVS_Id,CVS_Revision);
      (void)nc_lib_vrs_prn();
      exit(EXIT_SUCCESS);
      break;
    case 's': /* Copy the script for later processing */ 
      spt_arg[nbr_spt]=(char *)strdup(optarg);
      nbr_spt++;
      break;
    case 'S': /* Read the script from a file rather than command line */
      fl_spt=optarg;
      break;
    case 'v': /* Assemble the list of variables to extract/exclude */ 
      var_lst_in=lst_prs(optarg,",",&nbr_xtr);
      break;
    case 'x': /* Exclude rather than extract the variables specified with -v */
      EXCLUDE_INPUT_LIST=True;
      break;
    default: /* Print proper usage */
      (void)usg_prn();
      exit(EXIT_FAILURE);
      break;
    } /* end switch */
  } /* end while loop */
  
  /* Process positional arguments */ 
  if(optind < argc){
    int_foo=optind;
    (void)fprintf(stderr,"non-option ARGV-elements: ");
    while(int_foo < argc) (void)fprintf(stderr,"%s ",argv[int_foo++]);
    (void)fprintf(stderr,"\n");
  } /* end if */ 

  /* Process the positional arguments and fill in the filenames */
  fl_lst_in=fl_lst_mk(argv,argc,optind,&nbr_fl,&fl_out);

  /* Make a uniform list of the user-specified dimension limits */ 
  if(nbr_lmt > 0) lmt=lmt_prs(nbr_lmt,lmt_arg);
  
  /* Parse the filename */ 
  fl_in=fl_nm_prs(fl_in,0,&nbr_fl,fl_lst_in,nbr_abb_arg,fl_lst_abb,fl_pth);
  /* Make sure the file is on the local system and is readable or die trying */ 
  fl_in=fl_mk_lcl(fl_in,fl_pth_lcl,&FILE_RETRIEVED_FROM_REMOTE_LOCATION);
  /* Open the file for reading */ 
  rcd=nc_open(fl_in,NC_NOWRITE,&in_id);

  /* Get the number of variables, dimensions, and the record dimension ID for the file */
  rcd=nc_inq(in_id,&nbr_dim_fl,&nbr_var_fl,(int *)NULL,&rec_dim_id);
    
  /* Form the initial extraction list from the user input */
  xtr_lst=var_lst_mk(in_id,nbr_var_fl,var_lst_in,PROCESS_ALL_COORDINATES,&nbr_xtr);

  /* Change the included variables to excluded variables */ 
  if(EXCLUDE_INPUT_LIST) xtr_lst=var_lst_xcl(in_id,nbr_var_fl,xtr_lst,&nbr_xtr);

  /* Add all the coordinate variables to the extraction list */ 
  if(PROCESS_ALL_COORDINATES) xtr_lst=var_lst_add_crd(in_id,nbr_var_fl,nbr_dim_fl,xtr_lst,&nbr_xtr);

  /* Make sure all coordinates associated with each of the variables to be extracted is also on the list */ 
  if(PROCESS_ASSOCIATED_COORDINATES) xtr_lst=var_lst_ass_crd_add(in_id,xtr_lst,&nbr_xtr);

  /* Remove the record coordinate, if any, from the extraction list */ 
  if(False) xtr_lst=var_lst_crd_xcl(in_id,rec_dim_id,xtr_lst,&nbr_xtr);

  /* Finally, heapsort the extraction list by variable ID for fastest I/O */ 
  if(nbr_xtr > 1) xtr_lst=lst_heapsort(xtr_lst,nbr_xtr,False);
    
  /* We now have the final list of variables to extract. Phew. */
  /* Not! */ 

  /* Find the coordinate/dimension values associated with the limits */ 
  for(idx=0;idx<nbr_lmt;idx++) (void)lmt_evl(in_id,lmt+idx,0L,FORTRAN_STYLE);
  
  /* Find all the dimensions associated with all variables to be extracted */ 
  dim_lst=dim_lst_ass_var(in_id,xtr_lst,nbr_xtr,&nbr_dim_xtr);

  /* Fill in the dimension structure for all the extracted dimensions */ 
  dim=(dim_sct **)malloc(nbr_dim_xtr*sizeof(dim_sct *));
  for(idx=0;idx<nbr_dim_xtr;idx++) dim[idx]=dim_fll(in_id,dim_lst[idx].id,dim_lst[idx].nm);
  
  /* Merge the hyperslab limit information into the dimension structures */ 
  if(nbr_lmt > 0) (void)dim_lmt_merge(dim,nbr_dim_xtr,lmt,nbr_lmt);

  /* Duplicate the input dimension structures for output dimension structures */ 
  dim_out=(dim_sct **)malloc(nbr_dim_xtr*sizeof(dim_sct *));
  for(idx=0;idx<nbr_dim_xtr;idx++){
    dim_out[idx]=dim_dup(dim[idx]);
    (void)dim_xrf(dim[idx],dim_out[idx]); 
  } /* end loop over idx */

  /* Is this an NCAR CSM-format history tape? */
  NCAR_CSM_FORMAT=ncar_csm_inq(in_id);

  /* Fill in the variable structure list for all the extracted variables */ 
  var=(var_sct **)malloc(nbr_xtr*sizeof(var_sct *));
  var_out=(var_sct **)malloc(nbr_xtr*sizeof(var_sct *));
  for(idx=0;idx<nbr_xtr;idx++){
    var[idx]=var_fll(in_id,xtr_lst[idx].id,xtr_lst[idx].nm,dim,nbr_dim_xtr);
    var_out[idx]=var_dup(var[idx]);
    (void)var_xrf(var[idx],var_out[idx]);
    (void)var_dim_xrf(var_out[idx]);
  } /* end loop over idx */

  /* NB: ncap is not suited for var_lst_divide() */ 
  /* Divide the variable lists into lists of fixed variables and variables to be processed */ 
  (void)var_lst_divide(var,var_out,nbr_xtr,NCAR_CSM_FORMAT,(dim_sct **)NULL,0,&var_fix,&var_fix_out,&nbr_var_fix,&var_prc,&var_prc_out,&nbr_var_prc);

  /* Open the output file */ 
  fl_out_tmp=fl_out_open(fl_out,FORCE_APPEND,FORCE_OVERWRITE,&out_id);

  /* Copy all the global attributes */ 
  (void)att_cpy(in_id,out_id,NC_GLOBAL,NC_GLOBAL);
  
  /* Catenate the time-stamped command line to the "history" global attribute */ 
  if(HISTORY_APPEND) (void)hst_att_cat(out_id,cmd_ln);

  /* Define the dimensions in the output file */ 
  (void)dim_def(fl_out,out_id,dim_out,nbr_dim_xtr);

  /* Define the variables in the output file, and copy their attributes */ 
  (void)var_def(in_id,fl_out,out_id,var_out,nbr_xtr,(dim_sct **)NULL,0);

  /* Turn off the default filling behavior to enhance efficiency */ 
  rcd=nc_set_fill(out_id,NC_NOFILL,(int *)NULL);
  
  /* Take the output file out of define mode */ 
  rcd=nc_enddef(out_id);
  
  /* Zero the start vectors for all the output variables */ 
  (void)var_srt_zero(var_out,nbr_xtr);

  /* Copy the variable data for the non-processed variables */ 
  (void)var_val_cpy(in_id,out_id,var_fix,nbr_var_fix);

  (void)fnc_add("acos",acos);
  (void)fnc_add("asin",asin);
  (void)fnc_add("atan",atan);
  (void)fnc_add("cos",cos);
  (void)fnc_add("exp",exp);
  (void)fnc_add("gamma",gamma);
  (void)fnc_add("log",log);
  (void)fnc_add("log10",log10);
  (void)fnc_add("sin",sin);
  (void)fnc_add("sqrt",sqrt);
  (void)fnc_add("tan",tan);

  /* Set the arguments to the parser */ 
  prs_arg.fl_in=fl_in;
  prs_arg.in_id=in_id;
  prs_arg.fl_out=fl_out;
  prs_arg.out_id=out_id;
  prs_arg.sng=NULL;
  prs_arg.dim=dim;
  prs_arg.nbr_dim_xtr=nbr_dim_xtr;

  if(fl_spt == NULL){
    if(nbr_spt == 0){
      (void)fprintf(stderr,"%s: ERROR must supply derived field scripts\n",prg_nm_get());
      exit(EXIT_FAILURE);
    } /* end if */ 
    for(idx=0;idx<nbr_spt;idx++){
      if(dbg_lvl > 0) (void)fprintf(stderr,"spt_arg[%d]= %s\n",idx,spt_arg[idx]);
      /* Invoke the parser on the current script string */ 
      prs_arg.sng=spt_arg[idx];
      /* NB: Compiling ncap.y with Bison currently requires commenting out the prototype for yyparse() in bison.simple. 
	 This probably indicates a flaw in my understanding of Bison. */ 
      rcd=yyparse((void *)&prs_arg);
    } /* end for */ 
  }else{
    /* Open the script file for reading */ 
    if((yyin=fopen(fl_spt,"r")) == NULL){
      (void)fprintf(stderr,"%s: ERROR Unable to open script file %s\n",prg_nm_get(),fl_spt);
      exit(EXIT_FAILURE);
    } /* end if */ 
    /* Invoke the parser on the script file */ 
    rcd=yyparse((void *)&prs_arg);
  } /* end else */
  
  /* Close the input netCDF file */ 
  rcd=nc_close(in_id);
  
  /* Dispose of the local copy of the file */ 
  if(FILE_RETRIEVED_FROM_REMOTE_LOCATION && REMOVE_REMOTE_FILES_AFTER_PROCESSING) (void)fl_rm(fl_in);

  /* Close the output file and move it from the temporary to the permanent location */ 
  (void)fl_out_close(fl_out,fl_out_tmp,out_id);

  Exit_gracefully();

} /* end main() */ 

