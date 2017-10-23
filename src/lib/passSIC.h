#define tBufferSIZE 256

void pass0(operation *out, char in[], const char d[]) // split
{
	char a[3][sSIZE];
	memset(a, 0, sizeof a);
	int i = 0, k;
    for(char *t = strtok(in, d); t; t = strtok(NULL, d))
        strcpy(a[i++], strCapitalize(t));
        
    if((k = isAsmFunc(a[0]) == -1 && getOpcode(a[0]) == -1))
        strcpy(out->label, a[0]);
    
    strcpy(out->operator.mnemonic, a[k]);
    strcpy(out->operand, a[k + 1]);
}

void pass1(program *p) //LOC
{
    char *s = p->op[0].operator.mnemonic, *t;
    unsigned short LOCCTR = 0, i = 0;
    if(isStrEq(s, "START"))
    {
		sscanf(p->op[i].operand, "%hx", &LOCCTR);
		p->op[i++].loc = LOCCTR;
	}
    
    for(; i < p->len; i++)
    {
		p->op[i].loc = LOCCTR;
        s = p->op[i].operator.mnemonic;
        t = p->op[i].operand;
        
        if(isStrEq(s, "BYTE"))
			switch(t[0])
			{
				case 'X':
					LOCCTR += (strlen(t) - 2) / 2;
					break;
				case 'C':
					LOCCTR += strlen(t) - 3;
					break;
			}
        else if(isStrEq(s, "RESW")) LOCCTR += 3 * strInt(t);
        else if(isStrEq(s, "RESB")) LOCCTR += strInt(t);
        else LOCCTR += 3;
    }
    return;
}

void pass2(program *p) //generate object code
{
    char a[sSIZE], *s, *t, *r, charBuffer[sSIZE];
    unsigned short b;
    for(int i = 1; i < p->len - 1; i++)
    {
        s = p->op[i].operator.mnemonic;
        t = p->op[i].operand;
        r = p->op[i].objectcode;

        if(isStrEq(s, "BYTE"))
            switch(t[0])
            {
                case 'X':
                    sscanf(t, "X'%hx'", &b);
                    sprintf(r, "%02X", b); //cheap fix
                    break;
                case 'C':                    
                    sscanf(t, "C'%[^']'", a);
                    for(int j = 0; a[j] != '\0'; j++)
                    {
                        sprintf(charBuffer, "%02X", a[j]);
                        strcat(r, charBuffer);
                    }
                    break;
            }
        else if(isStrEq(s, "RESW"));
        else if(isStrEq(s, "RESB"));
        else if(isStrEq(s, "WORD"))
            sprintf(r, "%06X", strInt(t));
        else
        {
            strcpy(a, t);
            b = strReplace(a, ',');
            sprintf(r, "%06X", (getOpcode(s) << 16) | (b << 15) |
                                getLabelLocation(p, a));
        }
    }
    return;
}

void genHTE(program *p) //generate HTE Record
{
    operation *s = &p->op[0]; // may cause problemsm
    char tBuffer[tBufferSIZE], out[tBufferSIZE];
    short pSTART = s->loc, pLENGTH = (s + p->len - 1)->loc - pSTART,
          tBufferLoc = pSTART, tBufferLen = 0, l;

    sprintf(out, "H^%-6s^%06X", s->label, pLENGTH);
    printf("%s\n", out);

    while(!isStrEq(s++->operator.mnemonic, "END"))
    {
        l = strlen(s->objectcode) / 2;
        if((tBufferLen + l > rSIZE || !l) && tBufferLen) //rSIZE is 0x1E
        {
            sprintf(out, "T^%06X^%02X%s", tBufferLoc, tBufferLen, tBuffer);
            memset(tBuffer, tBufferLen = 0, tBufferSIZE); //size of what the pointer points... doesn't work
            printf("%s\n", out);
        }

        if(!l) continue;
        if(!tBufferLen) tBufferLoc = s->loc;
        strcat(tBuffer, "^");
        strcat(tBuffer, s->objectcode);
        tBufferLen += l;
    }

    sprintf(out, "E^%06X", pSTART);
    printf("%s\n", out);
}

