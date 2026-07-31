#include "platform_common.h"
#include "test.h"

#include "wdog_hal.h"
#include "gpio_hal.h"
#include "dhry.h"
#include "dhrystone_test.h"

#include <stdlib.h>

/* Global Variables: */

int             Int_Glob;
Boolean         Bool_Glob;
char            Ch_1_Glob,
                Ch_2_Glob;
AHB_SRAM_DATA_ATTRIBUTES int             Arr_1_Glob [50];
AHB_SRAM_DATA_ATTRIBUTES int             Arr_2_Glob [50] [50];

//extern char     *malloc ();
//Enumeration     Func_1();
/* forward declaration necessary since Enumeration may not simply be int */

Rec_Type Ptr_Glob = 
{
	.Discr = Ident_1,
};

Rec_Type Next_Ptr_Glob = 
{
	.Discr = Ident_1,
};

#ifndef REG
Boolean Reg = false;
#define REG
/* REG becomes defined as empty */
/* i.e. no register variables   */
#else
Boolean Reg = true;
#endif

/* variables for time measurement: */

#ifdef TIMES
/* see library function "times" */
#define Too_Small_Time 2
/* Measurements should last at least about 2 seconds */
#endif
#ifdef TIME
extern long     time(void);
/* see library function "time"  */
#define Too_Small_Time 2
/* Measurements should last at least 2 seconds */
#endif


extern Enumeration Func_1(Capital_Letter Ch_1_Par_Val, Capital_Letter Ch_2_Par_Val);
extern Boolean Func_2(Str_30 Str_1_Par_Ref, Str_30 Str_2_Par_Ref);
extern Boolean Func_3(Enumeration Enum_Par_Val);
extern void Proc_6(Enumeration  Enum_Val_Par, Enumeration* Enum_Ref_Par);
extern void Proc_7(One_Fifty Int_1_Par_Val, One_Fifty Int_2_Par_Val, One_Fifty* Int_Par_Ref);
extern void Proc_8(Arr_1_Dim Arr_1_Par_Ref, Arr_2_Dim Arr_2_Par_Ref, int Int_1_Par_Val, int Int_2_Par_Val);


long            Begin_Instret,
                End_Instret,
                User_Instret,
                Instret;
uint32_t        Begin_Time,
                End_Time;
double          User_Time;
double          Microseconds,
                Dhrystones_Per_Second,
								Vax_Mips;
float           DMIPS_MHZ;

/* end of variables for time measurement */
/* Only support dec number < 1000 */
//static char *dec2str(uint32_t val)
//{
//    static char str[4];
//    val = val % 1000;
//    int decnum = 100;
//    for (int i = 0; i < 3; i ++) {
//        str[i] = val / decnum + '0';
//        val = val % decnum;
//        decnum = decnum / 10;
//    }
//    str[3] = '\0';
//    return str;
//}

static uint32_t timer_begin(void)
{
	wdog_config_t wdog_config;

	wdog_config.wdog_clk_cfg.wdog_clk_div = WDOG_CLK_PSC_DIV256;
	wdog_config.wdog_clk_cfg.wdog_clk_sel = WDOG_CLK_SRC_LIRC;
	wdog_config.wdog_normal_timeout_value = LIRC_FREQ>>5U; /*LIRC_FREQ/256*8 = 8s*/
	wdog_config.wdog_window_timeout_value = 0U; 
	wdog_config.wdog_int_handle = NULL;
	wdog_config.wdog_int_enable = false;
  wdog_config.interrupt_priority = 0;	
	wdog_init(&wdog_config);
	uint32_t time_res = REG32(0x40004004);
	gpio_toggle(GPIO_A, GPIO_PIN_7);
	return time_res;
}

static uint32_t timer_get(void)
{
	gpio_toggle(GPIO_A, GPIO_PIN_7);
	uint32_t time_res = REG32(0x40004004);
	wdog_disable(true);
	return time_res;
}

void Proc_3 (Rec_Pointer *Ptr_Ref_Par)
{
    *Ptr_Ref_Par = Ptr_Glob.Ptr_Comp;
    Proc_7(10, Int_Glob, &(Ptr_Glob.variant.var_1.Int_Comp));
} /* Proc_3 */

void Proc_1 (REG Rec_Pointer Ptr_Val_Par)
/* executed once */
{
    REG Rec_Pointer Next_Record = Ptr_Val_Par->Ptr_Comp;
    /* == Ptr_Glob_Next */
    /* Local variable, initialized with Ptr_Val_Par->Ptr_Comp,    */
    /* corresponds to "rename" in Ada, "with" in Pascal           */

    structassign(*Ptr_Val_Par->Ptr_Comp, Ptr_Glob);
    Ptr_Val_Par->variant.var_1.Int_Comp = 5;
    Next_Record->variant.var_1.Int_Comp
        = Ptr_Val_Par->variant.var_1.Int_Comp;
    Next_Record->Ptr_Comp = Ptr_Val_Par->Ptr_Comp;
    Proc_3(&Next_Record->Ptr_Comp);
    /* Ptr_Val_Par->Ptr_Comp->Ptr_Comp
                        == Ptr_Glob->Ptr_Comp */
    if (Next_Record->Discr == Ident_1)
        /* then, executed */
    {
        Next_Record->variant.var_1.Int_Comp = 6;
        Proc_6(Ptr_Val_Par->variant.var_1.Enum_Comp,
               &Next_Record->variant.var_1.Enum_Comp);
        Next_Record->Ptr_Comp = Ptr_Glob.Ptr_Comp;
        Proc_7(Next_Record->variant.var_1.Int_Comp, 10,
               &Next_Record->variant.var_1.Int_Comp);
    } else { /* not executed */
        structassign(*Ptr_Val_Par, *Ptr_Val_Par->Ptr_Comp);
    }
} /* Proc_1 */


void Proc_2 (One_Fifty *Int_Par_Ref)
/* executed once */
/* *Int_Par_Ref == 1, becomes 4 */
{
    One_Fifty  Int_Loc;
    Enumeration   Enum_Loc;

    Int_Loc = *Int_Par_Ref + 10;
    do /* executed once */
        if (Ch_1_Glob == 'A')
            /* then, executed */
        {
            Int_Loc -= 1;
            *Int_Par_Ref = Int_Loc - Int_Glob;
            Enum_Loc = Ident_1;
        } /* if */
    while (Enum_Loc != Ident_1); /* true */
} /* Proc_2 */


void Proc_4()  /* without parameters */
/* executed once */
{
    Boolean Bool_Loc;

    Bool_Loc = Ch_1_Glob == 'A';
    Bool_Glob = Bool_Loc | Bool_Glob;
    Ch_2_Glob = 'B';
} /* Proc_4 */


void Proc_5()  /* without parameters */
/* executed once */
{
    Ch_1_Glob = 'A';
    Bool_Glob = false;
} /* Proc_5 */

/* main program, corresponds to procedures        */
/* Main and Proc_0 in the Ada version             */
void drystone_test(void)
{
          One_Fifty   Int_1_Loc;
    REG   One_Fifty   Int_2_Loc;
          One_Fifty   Int_3_Loc;
    REG   char        Ch_Index;
          Enumeration Enum_Loc;
          Str_30      Str_1_Loc;
          Str_30      Str_2_Loc;
    REG   int         Run_Index;
    REG   int         Number_Of_Runs;
          int         count = 10;

//    Next_Ptr_Glob = (Rec_Pointer)malloc(sizeof(Rec_Type));
//    Ptr_Glob = (Rec_Pointer)malloc(sizeof(Rec_Type));

    Ptr_Glob.Ptr_Comp                    = &Next_Ptr_Glob;
    Ptr_Glob.Discr                       = Ident_1;
    Ptr_Glob.variant.var_1.Enum_Comp     = Ident_3;
    Ptr_Glob.variant.var_1.Int_Comp      = 40;
    strcpy(Ptr_Glob.variant.var_1.Str_Comp,
           "DHRYSTONE PROGRAM, SOME STRING");
    strcpy(Str_1_Loc, "DHRYSTONE PROGRAM, 1'ST STRING");
		
		INFO("\n");
    INFO("Dhrystone Benchmark, Version 2.1 (Language: C)\n");
    INFO("\n");
    if (Reg) {
        INFO("Program compiled with 'register' attribute\n");
        INFO("\n");
    } else {
        INFO("Program compiled without 'register' attribute\n");
        INFO("\n");
    }
    INFO("Please give the number of runs through the benchmark: \n");
		
		Number_Of_Runs = 5000;
		INFO("\n");
    INFO("Execution starts, %d runs through Dhrystone\n", Number_Of_Runs);
		INFO("\n");
		
		
    do
    {
        Number_Of_Runs = Number_Of_Runs * 2;
        count = count - 1;
				Arr_2_Glob [8][7] = 10;

				Begin_Time = timer_begin();
        for (Run_Index = 1; Run_Index <= Number_Of_Runs; ++Run_Index)
        {
						Proc_5();
            Proc_4();
            /* Ch_1_Glob == 'A', Ch_2_Glob == 'B', Bool_Glob == true */
						Int_1_Loc = 2;
						Int_2_Loc = 3;
						strcpy(Str_2_Loc, "DHRYSTONE PROGRAM, 2'ND STRING");
						Enum_Loc = Ident_2;
						Bool_Glob = ! Func_2(Str_1_Loc, Str_2_Loc);
						/* Bool_Glob == 1 */
						while (Int_1_Loc < Int_2_Loc) { /* loop body executed once */
								Int_3_Loc = 5 * Int_1_Loc - Int_2_Loc;
								/* Int_3_Loc == 7 */
								Proc_7(Int_1_Loc, Int_2_Loc, &Int_3_Loc);
								/* Int_3_Loc == 7 */
								Int_1_Loc += 1;
						} /* while */
						/* Int_1_Loc == 3, Int_2_Loc == 3, Int_3_Loc == 7 */
						Proc_8(Arr_1_Glob, Arr_2_Glob, Int_1_Loc, Int_3_Loc);
						/* Int_Glob == 5 */
						Proc_1(&Ptr_Glob);
						for (Ch_Index = 'A'; Ch_Index <= Ch_2_Glob; ++Ch_Index)
								/* loop body executed twice */
						{
								if (Enum_Loc == Func_1(Ch_Index, 'C'))
										/* then, not executed */
								{
										Proc_6(Ident_1, &Enum_Loc);
										strcpy(Str_2_Loc, "DHRYSTONE PROGRAM, 3'RD STRING");
										Int_2_Loc = Run_Index;
										Int_Glob = Run_Index;
								}
						}
						/* Int_1_Loc == 3, Int_2_Loc == 3, Int_3_Loc == 7 */
						Int_2_Loc = Int_2_Loc * Int_1_Loc;
						Int_1_Loc = Int_2_Loc / Int_3_Loc;
						Int_2_Loc = 7 * (Int_2_Loc - Int_3_Loc) - Int_1_Loc;
						/* Int_1_Loc == 1, Int_2_Loc == 13, Int_3_Loc == 7 */
						Proc_2(&Int_1_Loc);
						/* Int_1_Loc == 5 */
        }
        End_Time = timer_get();
        User_Time = (((End_Time - Begin_Time)*256)/(double)(LIRC_FREQ));

        INFO("%12.0f runs %12.2f seconds \n",(double) Number_Of_Runs, User_Time);
        if (User_Time > 2)
        {
            count = 0;
        }
        else
        {
            if (User_Time < 0.05)
            {
                Number_Of_Runs = Number_Of_Runs * 5;
            }
        }
    }
    while (count >0);
		
		INFO("\n");
    INFO("Final values (* implementation-dependent):\n");
    INFO("\n");
    INFO("Int_Glob:      ");
    if (Int_Glob == 5)  INFO("O.K.  ");
    else                INFO("WRONG ");
    INFO("%d \n", Int_Glob);
      
    INFO("Bool_Glob:     ");
    if (Bool_Glob == 1) INFO("O.K.  ");
    else                INFO("WRONG ");
    INFO("%d \n", Bool_Glob);
      
    INFO("Ch_1_Glob:     ");
    if (Ch_1_Glob == 'A')  INFO("O.K.  ");               
    else                   INFO("WRONG ");
    INFO("%c \n", Ch_1_Glob);
         
    INFO("Ch_2_Glob:     ");
    if (Ch_2_Glob == 'B')  INFO("O.K.  ");
    else                   INFO("WRONG ");
    INFO("%c \n",  Ch_2_Glob);
   
    INFO("Arr_1_Glob[8]: ");
    if (Arr_1_Glob[8] == 7)  INFO("O.K.  ");
    else                     INFO("WRONG ");
    INFO("%d \n", Arr_1_Glob[8]);
            
    INFO("Arr_2_Glob8/7: ");
    if (Arr_2_Glob[8][7] == Number_Of_Runs + 10)
                          INFO("O.K.  ");
    else                   INFO("WRONG ");
    INFO("%d \n", Arr_2_Glob[8][7]);
   
    INFO("-------Ptr_Glob-------\n");
		INFO("Ptr_Comp: %d\n", (int) Ptr_Glob.Ptr_Comp);
    INFO("Discr:         ");
    if (Ptr_Glob.Discr == 0)  INFO("O.K.  ");
    else                       INFO("WRONG ");
    INFO("%d \n", Ptr_Glob.Discr);
            
    INFO("Enum_Comp:     ");
    if (Ptr_Glob.variant.var_1.Enum_Comp == 2)
                        INFO("O.K.  ");
    else                INFO("WRONG ");
    INFO("%d \n", Ptr_Glob.variant.var_1.Enum_Comp);
      
    INFO("Int_Comp:      ");
    if (Ptr_Glob.variant.var_1.Int_Comp == 17)  INFO("O.K.  ");
    else                                         INFO("WRONG ");
    INFO("%d \n", Ptr_Glob.variant.var_1.Int_Comp);
      
    INFO("Str_Comp:      ");
    if (strcmp(Ptr_Glob.variant.var_1.Str_Comp,
                        "DHRYSTONE PROGRAM, SOME STRING") == 0)
                        INFO("O.K.  ");
    else                INFO("WRONG ");   
    INFO("%s \n", Ptr_Glob.variant.var_1.Str_Comp);
   
    INFO("-------Next_Ptr_Glob-------\n");
		INFO("Ptr_Comp: %d", (int) Next_Ptr_Glob.Ptr_Comp);
    INFO(" same as above\n");
   
    INFO("Discr:         ");
    if (Next_Ptr_Glob.Discr == 0)
                        INFO("O.K.  ");
    else                INFO("WRONG ");
    INFO("%d \n", Next_Ptr_Glob.Discr);
   
    INFO("Enum_Comp:     ");
    if (Next_Ptr_Glob.variant.var_1.Enum_Comp == 1)
                        INFO("O.K.  ");
    else                INFO("WRONG ");
    INFO("%d \n", Next_Ptr_Glob.variant.var_1.Enum_Comp);
   
    INFO("Int_Comp:      ");
    if (Next_Ptr_Glob.variant.var_1.Int_Comp == 18)
                        INFO("O.K.  ");
    else                INFO("WRONG ");
    INFO("%d \n", Next_Ptr_Glob.variant.var_1.Int_Comp);
   
    INFO("Str_Comp:      ");
    if (strcmp(Next_Ptr_Glob.variant.var_1.Str_Comp,
                        "DHRYSTONE PROGRAM, SOME STRING") == 0)
                        INFO("O.K.  ");
    else                INFO("WRONG ");   
    INFO("%s \n", Next_Ptr_Glob.variant.var_1.Str_Comp);
   
    INFO("Int_1_Loc:     ");
    if (Int_1_Loc == 5)
                        INFO("O.K.  ");
    else                INFO("WRONG ");
    INFO("%d \n", Int_1_Loc);
      
    INFO("Int_2_Loc:     ");
    if (Int_2_Loc == 13)
                        INFO("O.K.  ");
    else                INFO("WRONG ");
    INFO("%d \n", Int_2_Loc);
   
    INFO("Int_3_Loc:     ");
    if (Int_3_Loc == 7)
                        INFO("O.K.  ");
    else                INFO("WRONG ");
    INFO("%d \n", Int_3_Loc);
   
    INFO("Enum_Loc:      ");
    if (Enum_Loc == 1)
                        INFO("O.K.  ");
    else                INFO("WRONG ");
    INFO("%d \n", Enum_Loc);
   
   
    INFO("Str_1_Loc:     ");
    if (strcmp(Str_1_Loc, "DHRYSTONE PROGRAM, 1'ST STRING") == 0)
                        INFO("O.K.  ");

    else                INFO("WRONG ");   
    INFO("%s \n", Str_1_Loc);

    INFO("Str_2_Loc:     ");
    if (strcmp(Str_2_Loc, "DHRYSTONE PROGRAM, 2'ND STRING") == 0)
                        INFO("O.K.  ");
    else                INFO("WRONG ");   
    INFO("%s \n", Str_2_Loc);
         
    INFO("\n");
		
    if (User_Time < Too_Small_Time)
    {
        INFO("Measured time too small to obtain meaningful results\n");
        INFO("Please increase number of runs\n");
        INFO("\n");
    }
    else
    {
        Microseconds = User_Time * Mic_secs_Per_Second / (double) Number_Of_Runs;
        Dhrystones_Per_Second = (double) Number_Of_Runs / User_Time;
        Vax_Mips = Dhrystones_Per_Second / 1757.0;

        INFO("Microseconds for one run: ");
        INFO("%.2lf \n", Microseconds);
        INFO("Dhrystones per Second: ");
        INFO("%.0lf \n", Dhrystones_Per_Second);
        INFO("VAX MIPS rating = ");
        INFO("%.2lf \n",Vax_Mips);
        INFO("\n");
 }
}



/* Procedure for the assignment of structures,          */
/* if the C compiler doesn't support this feature       */
#ifdef  NOSTRUCTASSIGN
sy_memcpy_s(d, l, s, l)
register char*   d;
register char*   s;
register int    l;
{
    while (l--) {
        *d++ = *s++;
    }
}
#endif
