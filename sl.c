#include <stdio.h>
#include <errno.h>   // for errno
#include <limits.h>  // for INT_MAX
#include <stdlib.h>  // for strtol
#include <math.h>  	// for fmaxf, must compile with -lm
#include <unistd.h>	// execl

// É coisa do passado, agora.
//#define TABELA_RET
#define PARCELA_ABATER
int main(int argc, char *argv[]){

struct esc
{
	float vl;
	float tx;
};

struct esc_pa
{
	float vl;
	float tx;
	float abat;	
};

// Escaloes IRS
#define nr_escal 8		// Numero escaloes
float ss_tr = 11;		// SS paga pelo trabalhador
float ss_em = 23.75;	// SS paga pelo empregador
//float sub_alim = 4.77;	// Subsídio alimentação, em cartão refeição seria até 7.63 isentos.
float sub_alim = 8.32;
float dias_trab = 231;	// Dias trabalho anuais: 21d * 11m (dias férias, CS)
float deduc_esp = 4104;	// Dedução específica (se < à SS)
float ret_fonte = 13.5;	// Retençao na fonte (13.5%)
float tx_adse = 3.5;		// Contribuição ADSE (3.5%)
// Actualizado 2ºsem2023

/*{
{886.57,	14.50},
{932.14,	21.00},
{999.14,	21.00},
{1106.93,	26.50},
{1600.36,	28.50},
{1961.36,	35.00},
{2529.05,	37.00},
{3694.46,	38.72},
{5469.90,	40.05},
{6420.55,	42.72},
{20064.21,	44.95},
{INT_MAX,	47.17}
};*/

// https://files.dre.pt/2s/2022/12/233000001/0001300021.pdf
// https://economiafinancas.com/wp-content/uploads/2023/01/Tabelas_RF_Continente_2_Semestre_2023_Portal.xlsx

struct esc_pa escal_pa[nr_escal]=
{
{1600.36,	28.50,	191.23},
{1961.36,	35.00,	295.26},
{2529.05,	37.00,	334.48},
{3694.46,	38.72,	377.86},
{5469.90,	40.05,	427.18},
{6420.55,	42.72,	573.22},
{20064.21,	44.95,	716.08},
{INT_MAX,	47.17,	1162.51}
};

// PRE 2023
/*struct esc escal[nr_escal]=
{
{7112,		14.5},
{10732,		23},
{20322,		28.5},
{25075,		35},
{36967,		37},
{80882,		45},
{INT_MAX,	48}
};*/

struct esc retf[25]=
{
{1005,		11.4},
{1065,		12.2},
{1143,		13.2},
{1225,		14.2},
{1321,		15.3},
{1424,		16.3},
{1562,		17.3},
{1711,		18.7},
{1870,		20.1},
{1977,		21.1},
{2090,		22.1},
{2218,		23.0},
{2367,		24.0},
{2535,		25.0},
{2767,		26.0},
{3104,		27.2},
{3534,		28.8},
{4118,		29.9},
{4650,		31.7},
{5194,		32.6},
{5880,		33.6},
{6727,		35.6},
{7939,		36.6},
{9560,		38.5},
{11282,		39.5}
};

// https://stackoverflow.com/questions/9748393/how-can-i-get-argv-as-int

char *p, is_anual = 0;
float liquido,imposto=0,salario, liq_menos_ret_fonte, colect, sstrab, ssempr, alim, adse;
int i = 0;
char out_str[1000], pwd[100], sh_path[100];
long conv;



errno = 0;
if (argc != 2)	{
	printf("\n Help! Help? no help\n");
	return 0;
}
if ( *argv[1] == 97) {				// ASCII 97 = "a"
	conv = strtol(argv[1] + 1, &p, 10);
	is_anual = 1;
} else {
	conv = strtol(argv[1], &p, 10);
}


// Check for errors: e.g., the string does not represent an integer
// or the integer is larger than int
if (errno != 0 || *p != '\0' || conv > INT_MAX) {
    // Put here the handling of the error, like exiting the seamonkey cookiesprogram with
    // an error message
	return 0;
} else {
    // No error
	if (is_anual == 0)
    	salario = (float) conv;
	else
		salario = (float) conv/14.;
	colect = salario*14;
	adse = colect * ( tx_adse/100);
	ssempr = (float) (ss_em/100)*colect;
	sstrab = (float) (ss_tr/100)*colect;
	//ssempr = (float) colect / (1 - ( ss_em / 100 )) - colect;
	//sstrab = (float) colect / (1 - ( ss_tr / 100 )) - colect;
	alim = dias_trab*sub_alim;

#ifdef TABELA_RET
	while (retf[i].vl < salario)
		i++;
	imposto = colect * (retf[i].tx/100);
	liquido = colect - imposto - sstrab;
	printf("Tabela Ret. ft.:%d %10.2f %10.2f", i, retf[i].vl, retf[i].tx );
#elif  defined(PARCELA_ABATER)
	while (escal_pa[i].vl < salario)
		i++;
	imposto = colect * (escal_pa[i].tx/100) - escal_pa[i].abat * 14;
	liquido = colect - imposto - sstrab;	
	printf("Tabela Ret. c/ parcela a abater ft.:%d %10.2f€ %10.2f%% %10.2f€", i, escal_pa[i].vl, escal_pa[i].tx, escal_pa[i].abat );	
#else
	colect -= fmaxf(deduc_esp, sstrab);
	//liq_menos_ret_fonte = colect * ( 1 - ( ret_fonte / 100 ) );
	while (colect > 0)
		{
			if (colect < escal[i].vl) {
				imposto+=(escal[i].tx/100)*colect;
				liquido=salario*14-sstrab-imposto;
				//liquido=salario*14-imposto;
				colect=0;
			} else {
				imposto+=(escal[i].tx/100)*escal[i].vl;
				colect-=escal[i].vl;
				i++;
			}
		}
#endif
	sprintf(out_str, "%10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f",salario*14, salario, liquido, liquido/14, imposto, imposto/14, alim, alim/12, sstrab, sstrab/14, ssempr, ssempr/14, (salario*14 + imposto + sstrab + ssempr) , (salario*14 + imposto + sstrab + ssempr)/14);
	//getcwd(pwd, 100);
	//sprintf(sh_path, "%s/dodialog.sh %s", pwd, out_str);
	//system(sh_path);

    printf("\nBRUTO\n\tAnual:%10.2f€ Mensal:%10.2f€\n", salario*14, salario);
	printf("LIQUIDO\n");
    printf("\tAnual:%10.2f€ Mensal:%10.2f€ \n", liquido, liquido/14);
	//printf("VENC. NA CONTA\n");
    //printf("\tAnual:%10.2f€ Mensal:%10.2f€ \n", liq_menos_ret_fonte, liq_menos_ret_fonte/14);
	//printf("DEVIDO NO FINAL DO ANO EM IRS\n");
    //printf("\tAnual:%10.2f€ Mensal:%10.2f€ \n", liq_menos_ret_fonte - liquido, (liq_menos_ret_fonte - liquido)/14);
	printf("IRS\n\tAnual:%10.2f€ Mensal:%10.2f€\n", imposto, imposto/14);
	printf("SEG SOCIAL TRAB - Anual:%10.2f€ Mensal:%10.2f€\n", sstrab, sstrab/14);		// pago pelo trabalhador (desconta do bruto)
	printf("SEG SOCIAL EMPR - Anual:%10.2f€ Mensal:%10.2f€\n", ssempr, ssempr/14);		// pago pelo empregador	(não desconta do bruto)
	printf("ALIMENTAÇÃO - Anual:%10.2f€ Mensal:%10.2f€\n\n", alim, alim/12);
	//printf("\tADSE\n");
    printf("ADSE anual:%10.2f€ Liq Anual:%10.2f€ Liq Mensal:%10.2f€ \n", adse, (liquido - adse), (liquido - adse)/14);
//	printf("CUSTO TOTAL EMPREGADOR Anual:%10.2f€ Mensal:%10.2f€ \n", (salario*14 + imposto + sstrab + ssempr) , (salario*14 + imposto + sstrab + ssempr)/14);
	printf("CUSTO TOTAL EMPREGADOR Anual:%10.2f€ Mensal:%10.2f€ \n", (salario*14 + ssempr) , (salario*14 + ssempr)/14);

}

}
// 2650b = 1800l, 3000b = 2000l, 3850b = 2500l
// Segurança Social é aparte de tudo, calculada sobre um putativo bruto+ss, que depois de descontada dá o bruto proper. Ou não.
