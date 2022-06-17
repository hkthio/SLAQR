getlist(list,type,vec,maxitems)
char *list, *type;
int *vec, maxitems;
   {
	int cnt, *ivec;
	float *fvec;
	char **svec;

	ivec= vec;
	fvec= (float *)vec;
	svec= (char **)vec;
	cnt= 0;
	while(cnt < maxitems && *list != '\0')
	   {
		while(*list == ' ' || *list == '\t') list++;
		switch(*type)
		   {
			case 'd':
				*ivec++ = atol(list);
				break;
			case 'f':
				*fvec++ = atof(list);
				break;
			case 's':
				*svec++ = list;
				break;
		   }
		while(*list != '\0' && *list != ',') list++;
		if(*list == ',') if(*type == 's') *list++ = '\0'; else list++;
		cnt++;
	   }
	return(cnt);
   }
getbool(list,name,defval)
char *list;
int defval;
   {
	char negname[32];
	sprintf(negname,"no%s",name);
	while(*list != '\0')
	   {
		while(*list == ' ' || *list == '\t') list++;
		if(posmatch(name,list)) defval= 1;
		if(negmatch(name,list)) defval= 0;
		while(*list != '\0' && *list != ',') list++;
		if(*list == ',') list++;
	   }
	return(defval);
   }

posmatch(s1,s2)
register char *s1, *s2;
   {
	while(*s2 != '\0' && *s2 != ',' && *s2 != ' ' && *s2 != '\t')
	   {
		if(*s1 != *s2) return(0);
		s1++;
		s2++;
	   }
	return(1);
   }

negmatch(s1,s2)
register char *s1, *s2;
   {
	if(s2[0] == 'n' && s2[1] == 'o' && posmatch(s1,&s2[2])) return(1);
	return(0);
   }
