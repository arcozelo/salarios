#!/bin/bash
br_a=$1
shift
br_m=$1
shift
lq_a=$1
shift
lq_m=$1
shift
ir_a=$1
shift
ir_m=$1
shift
al_a=$1
shift
al_m=$1
shift
sst_a=$1
shift
sst_m=$1
shift
sse_a=$1
shift
sse_m=$1
shift
ct_a=$1
shift
ct_m=$1




dialog --colors --title "BRUTO" --begin 2 2 --infobox "Anual: $br_a€\nMensal: $br_m€" 5 30 --and-widget \
		--colors --title "LIQUIDO" --begin 2 35 --infobox "Anual: $lq_a€\nMensal: $lq_m€" 5 30 --and-widget \
		--colors --title "IRS" --begin 9 2 --infobox "Anual: $ir_a€\nMensal: $ir_m€" 5 30 --and-widget \
		--colors --title "ALIMENTAÇÃO" --begin 9 35 --infobox "Anual: $al_a€\nMensal: $al_m€" 5 30 --and-widget \
		--colors --title "SEG SOCIAL TRABALHADOR" --begin 16 2 --infobox "Anual: $sst_a€\nMensal: $sst_m€" 5 30 --and-widget \
		--colors --title "SEG SOCIAL EMPREGADOR" --begin 16 35 --infobox "Anual: $sse_m€\nMensal: $sse_m€" 5 30 --and-widget \
		--colors --title "CUSTO TOTAL EMPREGADOR" --begin 23 2 --msgbox "Anual: $ct_a€\nMensal: $ct_m€" 6 63 
