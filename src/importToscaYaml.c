/***************************************************************************************
 *                                                                                     *
 * importToscaYaml.c: Set of functions specifically needed to implement the core       *
 *                    functionality of importToscaDescWinery application               *
 * Author             Roberto Mameli                                                   *
 * Version            v.1.0.0                                                          *
 * Date               September 2023                                                   *
 *                                                                                     *
 * ----------------------------------------------------------------------------------- *
 *                                                                                     *
 * Web:       https://www.eng.it/                                                      *
 * Linkedin:  https://www.linkedin.com/company/engineering-ingegneria-informatica-spa/ *
 * Twitter:   https://twitter.com/EngineeringSpa                                       *
 * Instagram: https://www.instagram.com/lifeatengineering/                             *
 *                                                                                     *
 * This contribution has been realized by Engineering Ingegneria Informatica S.p.A     *
 * and is available for free use under the terms of the Apache v.2.0 license (see      *
 * below)                                                                              *
 *                                                                                     *
 * ----------------------------------------------------------------------------------- *
 *                                                                                     *
 * Copyright 2023 Engineering Ingegneria Informatica S.p.A.                            *
 *                                                                                     *
 * Licensed under the Apache License, Version 2.0 (the "License");                     *
 * you may not use this file except in compliance with the License.                    *
 * You may obtain a copy of the License at                                             *
 *                                                                                     *
 *     http://www.apache.org/licenses/LICENSE-2.0                                      *
 *                                                                                     *
 * Unless required by applicable law or agreed to in writing, software                 *
 * distributed under the License is distributed on an "AS IS" BASIS,                   *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.            *
 * See the License for the specific language governing permissions and                 *
 * limitations under the License.                                                      *
 *                                                                                     *
 ***************************************************************************************/


/*****************
 * Include files *
 *****************/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "yaml.h"
#include "fmrt.h"

#include "importToscaDescWinery.h"


/********************
 * Global Variables *
 ********************/
shortString yamlMatchString[MAXTYPES] = {
    ARTYAMLMATCH, /* ARTIFACTTYPE     */
    CAPYAMLMATCH, /* CAPABILITYTYPE   */
    DATYAMLMATCH, /* DATATYPE         */
    GROYAMLMATCH, /* GROUPTYPE        */
    INTYAMLMATCH, /* INTERFACETYPE    */
    NODYAMLMATCH, /* NODETYPE         */
    POLYAMLMATCH, /* POLICYTYPE       */
    RELYAMLMATCH, /* RELATIONSHIPTYPE */
    TOPYAMLMATCH  /* TOPOLOGYTEMPTYPE */
};

shortString toscaType[MAXTYPES] = {
    ARTTYPE, /* ARTIFACTTYPE     */
    CAPTYPE, /* CAPABILITYTYPE   */
    DATTYPE, /* DATATYPE         */
    GROTYPE, /* GROUPTYPE        */
    INTTYPE, /* INTERFACETYPE    */
    NODTYPE, /* NODETYPE         */
    POLTYPE, /* POLICYTYPE       */
    RELTYPE, /* RELATIONSHIPTYPE */
    TOPTYPE  /* TOPOLOGYTEMPTYPE */
};

shortString topDir[MAXTYPES] = {
    ARTDIR, /* ARTIFACTTYPE     */
    CAPDIR, /* CAPABILITYTYPE   */
    DATDIR, /* DATATYPE         */
    GRODIR, /* GROUPTYPE        */
    INTDIR, /* INTERFACETYPE    */
    NODDIR, /* NODETYPE         */
    POLDIR, /* POLICYTYPE       */
    RELDIR, /* RELATIONSHIPTYPE */
    TOPDIR  /* TOPOLOGYTEMPTYPE */
};

shortString toscaFile[MAXTYPES] = {
    ARTTOSCAFILE, /* ARTIFACTTYPE     */
    CAPTOSCAFILE, /* CAPABILITYTYPE   */
    DATTOSCAFILE, /* DATATYPE         */
    GROTOSCAFILE, /* GROUPTYPE        */
    INTTOSCAFILE, /* INTERFACETYPE    */
    NODTOSCAFILE, /* NODETYPE         */
    POLTOSCAFILE, /* POLICYTYPE       */
    RELTOSCAFILE, /* RELATIONSHIPTYPE */
    TOPTOSCAFILE  /* TOPOLOGYTEMPTYPE */
};


/***********************************************************
 * This function replaces $ placeholder files in README.md *
 * with information parsed from yaml descriptor            *
 ***********************************************************/
void fillReadMe (FILE *readMeFptr, FILE *tempFptr, char *uri, char *name, char *type, char *description, char *derivedFrom)
{
    longString  lineString,
                tmpstring;
    char        *pp, *rr;

    while ( fgets(lineString,MAXLINE,readMeFptr) )
    {
        pp = lineString;
        tmpstring[0] = '\0';
        while ( (pp!=NULL) && (rr=strpbrk(pp,"$"))!= NULL )
        {
            *rr = '\0';
            strcat (tmpstring,pp);
            pp = rr+1;
            switch (*pp)
            {
                case '1':
                {   /* $1 = name + type (e.g. Root Node)*/
                    strcat (tmpstring,name);
                    strcat (tmpstring, " ");
                    strcat (tmpstring,type);
                    break;
                }   /* case '1' */
                case '2':
                {   /* $2 = description (e.g. This is the root node type.)*/
                    strcat (tmpstring,description);
                    break;
                }   /* case '2' */
                case '3':
                {   /* $3 = type name (e.g. Root) */
                    strcat (tmpstring,name);
                    break;
                }   /* case '3' */
                case '4':
                {   /* full Tosca type (e.g. tosca.nodes.Root) */
                    strcat (tmpstring,uri);
                    break;
                }   /* case '4' */
                case '5':
                {   /* $5 = derived from */
                    strcat (tmpstring,derivedFrom);
                    break;
                }   /* case '5' */
                default:
                    break;
            }   /* switch (*pp) */
            pp++;
        }   /* while ( (pp!=NULL) && (rr=strpbrk(pp,"$"))!= NULL ) */
        if (*pp != '\0')
            strcat (tmpstring,pp);
        fprintf (tempFptr,"%s",tmpstring);
    }   /* while ( fgets(lineString,MAXLINE,readMeFptr) ) */

    return;
}


/****************************************************************
 * This function creates the directory structure to store files *
 * used by Winery. On success it returns 0, -1 otherwise        *
 ****************************************************************/
int createDirStruct(char *topLevelDir, char *intLevelDir, char *fullDir)
{
    /* Local Variables */
    struct stat st = {0};

    /* Check and eventually create top level directory */
    if (stat(topLevelDir, &st) == -1)
        if ( mkdir(topLevelDir, DEFAULTMODE)==-1)
            return (-1);

    /* Check and eventually create intermediate level directory */
    if (stat(intLevelDir, &st) == -1)
        if ( mkdir(intLevelDir, DEFAULTMODE)==-1)
            return (-1);

    /* Check and eventually create top level directory */
    if (stat(fullDir, &st) == -1)
        if ( mkdir(fullDir, DEFAULTMODE)==-1)
            return (-1);

    return (0);
}


/*****************************************************************
 * This function copy files from the template directory into the *
 * target folder. On success it returns 0, -1 otherwise          *
 * NOTE WELL: in this version the content of the directory is    *
 * hardcoded and the system command is used to issue shell       *
 * commands (cp). This is a matter for further improvement in    *
 * future releases                                               *
 *****************************************************************/
int copyTemplates(int type, char *templateDir, char *targetDir)
{
    /* Local Variables */
    longString   cmd;

    /* Hardcoded -> Copy LICENSE file from template directory to final directory */
    sprintf (cmd,"cp %sLICENSE* %s",templateDir,targetDir);
    if (system (cmd))
        return (-1);
    
    /* Hardcoded -> Copy README file from template directory to final directory */
    sprintf (cmd,"cp %sREADME* %s",templateDir,targetDir);
    if (system (cmd))
        return (-1);

    /* Hardcoded -> Copy .tosca file from template directory to final directory */
    sprintf (cmd,"cp %s%s %s",templateDir,toscaFile[type],targetDir);
    if (system (cmd))
        return (-1);

    /* Hardcoded -> Copy /files subdirectory from template directory to final directory */
    sprintf (cmd,"cp -r %sfiles %s",templateDir,targetDir);
    if (system (cmd))
        return (-1);

    /* Hardcoded -> Copy /appearance subdirectory from template directory to final directory */
    sprintf (cmd,"cp -r %sappearance %s",templateDir,targetDir);
    if (system (cmd))
        return (-1);

    return (0);
}


/*****************************************************************
 * This function takes the first 3 arguments as input parameters *
 * (currentType, baseDir and uri extracted from the TOSCA file). *
 * It elaborates those information to determine name, type and   *
 * directory structure for the current element.                  *
 *****************************************************************/
void extractInfoFromUri(int currentType, char *baseDir, char *uri, char *name, char *type, char *file, char *topLevelDir, char *intLevelDir, char *fullDir)
{
	/* Local Variables*/
    int                 tokenNum;
    char               *p;
    fullToscaTypeString uriBkp;
    shortString         token[MAXTOKEN];

	/* Tokenize the string */
	tokenNum = 0;
	strcpy (uriBkp,uri);
	p = strtok(uriBkp,".");
	while (p)
	{
		strcpy (token[tokenNum++],p);
		p = strtok(NULL,".");
	}
	
	/* Evaluate relevant information from the tokens previously extracted (name, dir, etc.) */
	strcpy (name,token[tokenNum-1]);
	strcpy (type,toscaType[currentType]);
	strcpy (file,toscaFile[currentType]);
	
	/* Compose properly the directory structure */
	p = uri + strlen(uri)-1;
	while (*p != '.')
		p--;
	*p='\0';
	sprintf (topLevelDir, "%s%s", baseDir,topDir[currentType]);
	sprintf (intLevelDir, "%s%s", topLevelDir,uri);
	sprintf (fullDir,"%s/%s",intLevelDir,token[tokenNum-1]);
	*p = '.';
	
	return;

}


/*****************************************************************
 * This function takes the input yaml file and isolates the last *
 * part (i.e. the file name at the bottom level, after the last  *
 * slash. Checks that the file name terminates with .yml or .yaml*
 * and, if so, uses it as name of the Service Template           *
 *****************************************************************/
int extractTopologyTemplateName(char *inputYamlFile, char *name)
{
    /* Local Variables */
    fullFileNameString  fileName;
    char               *p;

    /* Preliinary Chacks */
    if ( (strlen(inputYamlFile)<1) || (strlen(inputYamlFile)>DIRLENGTH) || (strpbrk(inputYamlFile,"|!\"£$%()=?\'^\\[]*+@#;:,<>&") != NULL) )
    {
        printf ("Fatal Error... Input YAML File Name is either too short, too long or contains invalid characters\n");
        return (-1);
    }   /* if ( (strlen(fileName)<1) || ... */

    /* Copy the name into another string to avoid corruption */
    /* Truncate it if too long */
    strncpy (fileName, inputYamlFile,DIRLENGTH+1);
    fileName[DIRLENGTH+1] = '\0';

    /* Check that the file name has extension .yaml or .yml */
    p = strrchr(fileName,'.');     /* Locate the last occurrence of '.' in input yaml filename */
    if ( (p==NULL) || ((strcasecmp(p+1,"yaml")) && (strcasecmp(p+1,"yml"))) )
    {
        printf ("Fatal Error... input YAML file containing a topology_template shall have .yaml o .yml extension");
        return (-1);
    }   /* if ( (p==NULL) || (strcasecmp(*(p+1),"yaml")) || (strcasecmp(*(p+1),"yml")) ) */

    /* extract the name */
    *p = '\0';
    p = strrchr(fileName,'/');     /* Locate the last occurrence of '/' in input yaml filename */
    if (p==NULL)
        strcpy (name,fileName);
    else
        strcpy (name, p+1);

    /* Final check on name length */
    if ( (strlen(name)<1) || (strlen(name)>NAMELENGTH) )
    {
        printf ("Fatal Error... Resulting Name is either too short or too long\n");
        return (-1);
    }   /* if ( (strlen(fileName)<1) || ... */

    /* Everything OK */
    return (0);
}


/*****************************************************************
 * This function append a text file (whose name is given by the  *
 * second parameter) to another text file (name in first param)  *
 * NOTE WELL: in this version the copy is done by issuing a shell*
 * command (cat <append> >> <target>), which is quite ugly. This *
 * is a matter for further improvement in future releases        *                                               *
 *****************************************************************/
int appendMetaData (char * target, char * append)
{
    /* Local Variables */
    longString   cmd;

    /* Hardcoded -> append files through a shell command */
    sprintf (cmd,"cat %s >> %s",append,target);
    if (system (cmd))
        return (-1);

    return (0);
}