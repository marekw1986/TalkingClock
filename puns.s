PA1 = $00 ; before BB,DD,GG,JH (10 msec)
PA2 = $01 ; before BB,DD,GG,JH (30 msec)
PA3 = $02 ; before PP,TT,KK,CH and between words (50 msec)
PA4 = $03 ; between clauses and sentences (100 msec)
PA5 = $04 ; between clauses and sentences (200 msec)

; Short Vowels
IH  = $0c ; sIt
EH  = $07 ; End, gEntlemen, Extend
AE  = $1a ; hAt, extrAct, Act
UH  = $1e ; bOOk, cOOkie, fUll
AO  = $17 ; tALk, sOng, AUght
AX  = $0f ; sUCCeed, lApel, intrUCt
AA  = $18 ; hOT, pOTtery, cOTen

; Long Vowels
IY  = $13 ; sEE, trEAt, pEOple, pEnny
EY  = $14 ; bEIge, grEAt, stAte, trAY
AY  = $06 ; skY, kIte, mIGHty
OY  = $05 ; bOY, nOIse, vOIce
UW1 = $16 ; compUter (after clusters with YY)
UW2 = $1f ; fOOd, tWO (monosyllabic words)
OW  = $35 ; zOne, clOse, snOW, bEAU
AW  = $20 ; OUt, sOUnd, mOUse, dOWn
EL  = $3e ; saddLE, littLE, angLE, gentLEmen

; R-Colored Vowels
ER1 = $33 ; lettER, furnitURE, intERRupt
ER2 = $34 ; bIRd, fERn, bURn (monosyllabic words)
OR  = $3a ; stORe, adORn, fORtune
AR  = $3b ; alARm, fARm, gARment
YR  = $3c ; clEAR, EARring, IResponsible
XR  = $2f ; repaiR, haiR, declaRE, staRE

; Resonants
WW  = $2e ; Wool, We, lingUIst
RR1 = $0e ; Rural, WRite, x-Ray (initial position)
RR2 = $27 ; bRain, bRown, gRease (initial clusters)
LL  = $2d ; Lake, Like, heLLo, steeL
YY1 = $31 ; cUte, bEAUty, compUter (clusters)
YY2 = $19 ; Yes, Yarn, Yo-Yo (initial position)

; Voiced Fricatives
VV  = $23 ; Vest, proVE, eVen
DH1 = $12 ; THis, THen, THey (initial position)
DH2 = $36 ; baTHe, baTHing (word-final and between vowels)
ZZ  = $2b ; Zoo, phaSE
ZH  = $26 ; aZure, beiGE, pleaSUre

; Voiceless Fricatives
FF  = $28 ; Food (can be doubled for initial position)
TH  = $1d ; THin (can be doubled for initial position)
SS  = $37 ; Snake, veSt (can be doubled for initial position)
SH  = $25 ; SHip, SHirt, leaSH, naTion
HH1 = $1b ; He (before front vowels YR,IY,IH,EY,EH,XR,AE)
HH2 = $39 ; Hoe (before back vowels UW,UH,OW,OY,AO,OR,AR)
WH  = $30 ; WHig, WHite, tWenty

; Voiced Stops
BB1 = $1c ; riB, fiBBer, (in clusters) BLeed, BRown
BB2 = $3f ; Business, Beast (initial position before vowel)
DD1 = $15 ; coulD, playeD, enD (final position)
DD2 = $21 ; Do, Down, Drain (initial position and clusters)
GG1 = $24 ; Got (before high front vowels YR,IY,IH,EY,EH,XR)
GG2 = $3d ; GUest, Glue, Green (before high back vowels UW,UH,OW,OY,AX)
GG3 = $22 ; wiG, anGer (before low vowels AE,AW,AY,AR,AA,AO,OR,ER)

; Voiceless Stops
PP  = $09 ; Pow, triP, amPle, Pleasure
TT1 = $11 ; parTs, tesTs, iTs (final cluster before SS)
TT2 = $0d ; To, TesT, sTreeT (all positions except before final SS)
KK1 = $2a ; Can't, Cute, Clown, sCream (before front vowels YR,IY,IH,EY,EH,XR,AY,AE,ER,AX and initial clusters)
KK2 = $29 ; speaK, tasK (final position or final cluster)
KK3 = $08 ; Comb, QUick, Crane, sCream (before back vowels UW,UH,OW,OY,OR,AR,AO and initial clusters)

; Affricates
CH  = $32 ; CHurCH, feaTure
JH  = $0a ; JudGE, inJUre, dodGE

; Nasal
MM  = $10 ; Milk, alarM, aMple
NN1 = $0b ; thiN, earN (before front and central vowels YR,IY,IH,EY,EH,XR,AE,ER,AX,AW,AY, and final clusters)
NN2 = $38 ; No (before back vowels UH,OW,OY,OR,AR,AA)
NG  = $2c ; striNG, aNGer, aNchor

STOP = $F0

.org $2000
INDEX:
	.dbyt	ASTALAVISTA, HELLOWORLD, MAYTHEFORCE, EXTERMINATE, ILLBEBACK, $00
ASTALAVISTA:
	.byte	AE, SS, PA3, TT1, AE, LL, AE, VV, IH, SS, PA3, TT1, AX, PA3, PA1, BB1, EY, PA1, BB1, IY, PA3, STOP
HELLOWORLD:
	.byte	HH1, EH, LL, OW, PA3, WW, ER1, LL, PA2, DD1, PA3
MAYTHEFORCE:
	.byte	MM, EY, PA3, DH1, AX, PA3, FF, AO, RR2, SS, PA3, PA1, BB1, IY, PA3, WW, IH, TH, PA3, YY1, UW1, PA3, STOP
EXTERMNATE:
	.byte	EH, PA2, KK1, SS, PA3 TT1, ER1, MM, IH, NN1, EY, PA3 TT1, PA3, STOP
ILLBEBACK:
	.byte	AY, LL, PA3, PA1, BB1, IY, PA3, PA1, BB1, AE, PA2, KK1, PA3, STOP
