/***************************************************************************************
 *                                                                                     *
 * importToscaDescWinery.c: This application parses a tosca descriptor in yaml format  *
 *                          and organizes information contained therein into a         *
 *                          directory suitable for Eclipse Winery                      *
 * Author                   Roberto Mameli                                             *
 * Version                  v.1.0.0                                                    *
 * Date                     September 2023                                             *
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
extern shortString yamlMatchString[];
extern shortString toscaType[];
extern shortString topDir[];
extern shortString toscaFile[];


/*****************
 * MAIN FUNCTION *
 *****************/
int main (int argc, char ** argv)
{
    /* Main Function - Local Variables*/
    int                 i, currentLevel,
                        currentType,
                        lastKey,
                        yamlContent,
                        result;
    bool                topLevelMetadata;
    char               *p, *q;
    fullFileNameString  inputYamlFile = "",
                        templateDir = "",
                        baseDir = DEFBASEDIR,
                        topLevelDir,
                        intLevelDir,
                        fullDir,
                        tempFileName,
                        readMeFileName,
                        topLevelMetadataFileName = "",
                        toscaFileName="";
    longString          description,
                        derivedFrom,
                        lineString;
    fullToscaTypeString uri,
                        targetNameSpace;
    typeNameString      name;
    shortFileNameString file;
    typeString          type;
    FILE               *inputYamlFD;
    yaml_parser_t       parser;     /* https://www.wpsoftware.net/andrew/pages/libyaml.html */
    yaml_event_t        event;
    FILE               *tempFptr,
                       *readMeFptr,
                       *yamlFptr,
                       *topLevelMetadataFptr,
                       *toscaFptr=NULL;
    long                currentFilePos;
    fmrtResult          res;

    /* --------------------------------------- */
    /* Step 1 - Command Line Arguments Parsing */
    /* --------------------------------------- */
    if ( (argc!=5)&&(argc!=7) )
    {
        printUsage();
        exit (-1);
    }
    for (i=1;i<argc;i++)
    {
        if (argv[i][0]!='-')
        {
            printUsage();
            exit (-1);            
        }   /* if (argv[i][0]!='-') */
        switch (argv[i][1])
        {
            case 'i':
            {
                i +=1;
                strcpy (inputYamlFile,argv[i]);
                break;
            }   /* case 'i': */
            case 'o':
            {
                i +=1;
                strcpy (baseDir,argv[i]);
                /* Make sure that input dir terminates always with slash */
                if ( baseDir[strlen(baseDir)-1]!='/')
                    strcat (baseDir,"/");
                break;
            }   /* case 'o': */
            case 't':
            {
                i +=1;
                strcpy (templateDir,argv[i]);
                /* Make sure that template dir terminates always with slash */
                if ( templateDir[strlen(templateDir)-1]!='/')
                    strcat (templateDir,"/");
                break;
            }   /* case 't': */
            default:
            {   /* Unexpected option */
                printUsage();
                exit(-1);
            }   /* default */
        }   /* switch (argv[i][1]) */
    }   /* for (i=1;i<argc;i++) */
    if ( (inputYamlFile[0]=='\0') || (templateDir[0]=='\0'))
    {   /* One of the mandatory parameter is missing */
        printUsage();
        exit(-1);
    }

    #ifdef DEBUG
    printf ("<yaml_tosca_descriptor>: %s\n",inputYamlFile);
    printf ("<template_dir>: %s\n",templateDir);
    printf ("<winery_base_dir>: %s\n\n",baseDir);
    #endif

    /* ---------------------------------------------------------------- */
    /* Step 2 - Invoke fmrtxxx() library calls to define the table used */
    /* to store information read from input yaml file. This table will  */
    /* be used in the second pass on the yaml file, to fill the content */
    /* of the *.tosca files                                             */
    /* ---------------------------------------------------------------- */
    if ( (res=fmrtDefineTable(TABLEID,TABLENAME,MAXYAMLTYPES)) != FMRTOK)
    {
        printFmrtLibError (res);
        exit(-1);
    }
    if ( (res=fmrtDefineKey(TABLEID,URINAME,FMRTSTRING,URILENGTH)) != FMRTOK)
    {
        printFmrtLibError (res);
        exit(-1);
    }
    if ( (res=fmrtDefineFields(TABLEID,NUMFIELDS,
                               NAMENAME,FMRTSTRING,NAMELENGTH,
                               TYPEINTNAME,FMRTINT,
                               TYPENAME,FMRTSTRING,TYPELENGTH,
                               FILENAME,FMRTSTRING,FILELENGTH,
                               DIRNAME,FMRTSTRING,DIRLENGTH)) != FMRTOK)
    {
        printFmrtLibError (res);
        exit(-1);
    }

    /* --------------------------------------- */
    /* Step 3 - Preliminary libyaml operations */
    /* --------------------------------------- */
    /* Open YAML descriptor */
    if ( (inputYamlFD=fopen(inputYamlFile,"r"))==NULL)
    {
        printf ("Fatal Error... Unable to open input yaml descriptor\n");
        exit(-1);
    }
    /* Initialize libyaml parser */
    if(!yaml_parser_initialize(&parser))
    {
        printf ("Fatal Error... Failed to initialize yaml parser\n");
        exit(-1);
    }
    /* Set input file */
    yaml_parser_set_input_file(&parser, inputYamlFD);

    /* ------------------------------------------------------------- */
    /* Step 4 - Parse the yaml file by using libyaml functions       */
    /* Extract relevant information and prepare target dir structure */
    /* ------------------------------------------------------------- */
    /* Some initialization before parsing yaml imput file */
    currentLevel = DOCUMENT;
    currentType = NULLTYPE;
    strcpy (targetNameSpace,DEFTARGETNS);
    strcpy (description,DEFDESCRIPTION);
    strcpy (derivedFrom,DEFDERIVEDFROM);
    lastKey = NULLKEY;
    yamlContent = UNKNOWN;      /* we don't know yet if the file contains new types or a topology */
    topLevelMetadata = false;   /* set to true in case of metadata at the top level in input file */
    /* Start parsing through libyaml ibrary functions */
    do
    {
        if (!yaml_parser_parse(&parser, &event))
        {
           printf ("Fatal Error... Parser error %d\n", parser.error);
           exit(-1);
        }
  
        switch(event.type)
        { 
            case YAML_STREAM_START_EVENT:
            {
                if (currentLevel != DOCUMENT)
                {
                    printf ("Fatal Error... Unexpected Stream Start Event\n");
                    exit(-1);
                }
                #ifdef DEBUG
                printf ("STREAM START...\n");
                #endif
                break;
            }   /* case YAML_STREAM_START_EVENT: */
            case YAML_STREAM_END_EVENT:
            {
                if (currentLevel != DOCUMENT)
                {
                    printf ("Fatal Error... Unexpected Stream End Event\n");
                    exit(-1);
                }
                #ifdef DEBUG
                printf ("STREAM END...\n");
                #endif
                break;
            }   /* case YAML_STREAM_END_EVENT */
            case YAML_DOCUMENT_START_EVENT:
            {
                if (currentLevel != DOCUMENT)
                {
                    printf ("Fatal Error... Unexpected Document Start Event\n");
                    exit(-1);
                }
                #ifdef DEBUG
                printf ("  DOCUMENT START...\n");
                #endif
                break;
            }   /* case YAML_DOCUMENT_START_EVENT: */
            case YAML_DOCUMENT_END_EVENT:
            {
                if (currentLevel != DOCUMENT)
                {
                    printf ("Fatal Error... Unexpected Document End Event\n");
                    exit(-1);
                }
                #ifdef DEBUG
                printf ("  DOCUMENT END...\n");
                #endif
                break;
            }   /* case YAML_DOCUMENT_END_EVENT */
            case YAML_MAPPING_START_EVENT:
            {
                currentLevel+=1;
                #ifdef DEBUG
                printf ("    Start Mapping (Key:Value pairs)... level=%d\n",currentLevel);
                #endif
                break;
            }   /* case YAML_MAPPING_START_EVENT: */
            case YAML_MAPPING_END_EVENT:
            {
                currentLevel-=1;
                #ifdef DEBUG
                printf ("    End Mapping (Key:Value pairs)... level=%d\n",currentLevel);
                #endif
                switch (currentLevel)
                {
                    case MAINTYPE:
                    {   /* mapping terminated at MAINTYPE level */
                        /* (applicable for topology_tempolate)  */
                        switch (currentType)
                        {
                            case TOPOLOGYTEMPTYPE:
                            {   /* handling of topology_template terminated --> replace placeholders */
                                /* in the README.md file with values read from the yaml file         */
                                sprintf (readMeFileName,"%s/README.md",fullDir);
                                sprintf (tempFileName,"%s/.README.md.tmp",fullDir);
                                if ( (tempFptr=fopen(tempFileName,"w"))==NULL )
                                {
                                    printf ("Unable to edit README File - Unrecoverable Error\n");
                                    exit (0);
                                }
                                if ( (readMeFptr=fopen(readMeFileName,"r"))==NULL )
                                {
                                    printf ("Unable to open README File - Unrecoverable Error\n");
                                    exit (0);
                                }
                                /* Fill README.md file by substituting $ placeholders with relevant info */
                                fillReadMe (readMeFptr, tempFptr, targetNameSpace, name, toscaType[currentType], description, derivedFrom);
                                fclose (tempFptr);
                                fclose (readMeFptr);
                                rename (tempFileName,readMeFileName);
                                #ifdef DEBUG
                                printf("      README.md file enriched with information from yaml descriptor\n");
                                printf("        targetNameSpace: %s\n",targetNameSpace);
                                printf("        Short Name: %s\n",name);
                                printf("        Type: %s\n",toscaType[currentType]);
                                printf("        Top Level Dir: %s\n",topLevelDir);
                                printf("        Intermediate Dir: %s\n",intLevelDir);
                                printf("        Full Dir: %s\n", fullDir);
                                printf("        Description: %s\n", description);
                                #endif
                                break;
                            }   /* case TOPOLOGYTEMPTYPE */
                            default:
                                /* skip if we are not handling a TOSCA type */
                                break;
                        }   /* switch (currentType) */
                        break;
                    }   /* case MAINTYPE: */
                    case TOSCATYPE:
                    {   /* mapping terminated at TOSCATYPE level         */
                        /* (applicable for data_types, node_types, etc.) */
                        switch (currentType)
                        {
                            case NULLTYPE:
                            case TOPOLOGYTEMPTYPE:
                                /* skip if we are not handling a TOSCA type */
                                break;
                            default:
                            {   /* handling of TOSCA type terminated --> replace placeholders */
                                /* in the README.md file with values read from the yaml file  */
                                sprintf (readMeFileName,"%s/README.md",fullDir);
                                sprintf (tempFileName,"%s/.README.md.tmp",fullDir);
                                if ( (tempFptr=fopen(tempFileName,"w"))==NULL )
                                {
                                    printf ("Unable to edit README File - Unrecoverable Error\n");
                                    exit (0);
                                }
                                if ( (readMeFptr=fopen(readMeFileName,"r"))==NULL )
                                {
                                    printf ("Unable to open README File - Unrecoverable Error\n");
                                    exit (0);
                                }
                                /* Fill README.md file by substituting $ placeholders with relevant info */
                                fillReadMe (readMeFptr, tempFptr, uri, name, type, description, derivedFrom);
                                fclose (tempFptr);
                                fclose (readMeFptr);
                                rename (tempFileName,readMeFileName);
                                #ifdef DEBUG
                                printf("      README.md file enriched with information from yaml descriptor\n");
                                printf("        Full Tosca Type: %s\n",uri);
                                printf("        Short Name: %s\n",name);
                                printf("        Type: %s\n",toscaType[currentType]);
                                printf("        Top Level Dir: %s\n",topLevelDir);
                                printf("        Intermediate Dir: %s\n",intLevelDir);
                                printf("        Full Dir: %s\n", fullDir);
                                printf("        Derived From: %s\n", derivedFrom);
                                printf("        Description: %s\n", description);
                                #endif
                                break;
                            }   /* default */
                        }   /* switch (currentType) */
                        break;
                    }   /* case TOSCATYPE: */
                    default:
                    {   /* Nothing to do at this level */
                        break;
                    }   /* default */
                }   /* switch (currentLevel) */
                break;
            }   /* case YAML_MAPPING_END_EVENT: */
            case YAML_SEQUENCE_START_EVENT:
            {   /* This identifies the start of a list -> not handled for our purpose */
                #ifdef DEBUG
                printf("    Start Sequence at level=%d\n",currentLevel);
                #endif
                break;
            }   /* YAML_SEQUENCE_START_EVENT: */
            case YAML_SEQUENCE_END_EVENT:
            {   /* This identifies the end of a list -> not handled for our purpose */
                #ifdef DEBUG
                printf("    End Sequence at level=%d\n",currentLevel);
                #endif
                break;
            }   /* YAML_SEQUENCE_END_EVENT: */
            case YAML_ALIAS_EVENT:
            {   /* Do we need to manage this event? */
                #ifdef DEBUG
                printf("Got alias (anchor %s) at level=%d\n", event.data.alias.anchor,currentLevel);
                #endif
                break;
            }   /* case YAML_ALIAS_EVENT: */
            case YAML_SCALAR_EVENT:
            {   /* This is the relevant event that shall be managed */
                /* Processing depends on the currentLevel */
                #ifdef DEBUG
                printf("Got scalar (value %s) at level=%d\n", event.data.scalar.value,currentLevel);
                #endif
                /* Logic depending on the current level */
                switch (currentLevel)
                {
                    case MAINTYPE:
                    {   /* Top level scalar values (i.e. without indentation). Those include things such as data_type, */
                        /* node_type, etc., topology_template, or even metadata, description at the highest level     */
                        /* Is this s known type, a topology_template or something else? */
                        for (i=0; i<MAXTYPES; i++)
                            if ( strcmp(yamlMatchString[i],(char*)event.data.scalar.value)==0 )
                                break;
                        if (i<MAXTYPES)
                        {
                            currentType = i;
                            #ifdef DEBUG
                            printf ("  CurrentType = %s\n",toscaType[currentType]);
                            #endif
                        }   /* if (i<MAXTYPES) */
                        else
                        {
                            currentType = NULLTYPE;
                            #ifdef DEBUG
                            printf ("  CurrentType = NOT MEANINGFUL\n");
                            #endif
                        }   /* else if (i<MAXTYPES) */
                        /* Implement different operations depending on currentType */
                        switch (currentType)
                        {
                            case NULLTYPE:
                            {
                                if (strcmp(METADATA,(char*)event.data.scalar.value)==0)
                                {   /* Found metadata at the main level -> set topLevelMetadata */
                                    topLevelMetadata = true;
                                    sprintf (topLevelMetadataFileName,"%s.toplevel.metadata",baseDir);
                                    /* Not sure if the following initialization is really needed, */
                                    /* but it does not harm so let's keep it anyway               */
                                    lastKey = NULLKEY;
                                    strcpy (targetNameSpace,DEFTARGETNS);
                                }   /* if (strcmp(METADATA,(char*)event.data.scalar.value)==0) */
                                if (lastKey==DESCRIPTIONKEY)
                                {   /* Found description at the main level */
                                    sprintf (description,"%s",(char*)event.data.scalar.value);
                                    lastKey=NULLKEY;
                                }   /* if (lastKey==DESCRIPTIONKEY) */
                                if ( (strcmp(DESCRIPTION,(char*)event.data.scalar.value)==0) && (lastKey==NULLKEY) )
                                    /* Found description at the main level */
                                    lastKey = DESCRIPTIONKEY;
                                break;
                            }   /* case NULLTYPE */
                            case TOPOLOGYTEMPTYPE:
                            {   /* Check that no types have been defined in the same file */
                                switch (yamlContent)
                                {
                                    case TOPOLOGY:
                                    {
                                        printf ("Fatal Error... the input yaml file contains 2 Topology Templates (only one is allowed)\n");
                                        exit (-1);
                                    }   /* case TOPOLOGY */
                                    case TYPES:
                                    {
                                        printf ("Fatal Error... the input yaml file cannot contain type definitions and a topology at the same time\n");
                                        exit (-1);
                                    }   /* case TYPES */
                                    default:
                                        break;
                                }   /* switch (yamlContent) */
                                /* Evaluate name, type, file, and all directories */
                                yamlContent = TOPOLOGY;
                                if ( (result=extractTopologyTemplateName(inputYamlFile,name)) != 0)
                                    exit (-1);
                                sprintf (topLevelDir, "%s%s", baseDir,topDir[currentType]);
                                sprintf (intLevelDir, "%s%s", topLevelDir,targetNameSpace);
                                sprintf (fullDir,"%s/%s",intLevelDir,name);
                                /* Insert the topology_template key word just read into the internal fmrt cache for the second pass */
                                switch (res=fmrtCreate(TABLEID,
                                                       yamlMatchString[currentType],
                                                       name,
                                                       currentType,
                                                       toscaType[currentType],
                                                       toscaFile[currentType],
                                                       fullDir) )
                                {
                                    case FMRTOK:
                                        break;
                                    case FMRTDUPLICATEKEY:
                                    {   /* This should never happen, since this check has been already done some lines above */
                                        printf ("Fatal Error... the input yaml file contains 2 Topology Templates (only one is allowed)\n");
                                        exit (-1);
                                    }   /* case FMRTDUPLICATEKEY: */
                                    default:
                                    {
                                        printFmrtLibError (res);
                                        exit(-1);
                                    }   /* default */
                                }   /* switch (res=fmrtCreate(TABLEID,... */
                                if (createDirStruct(topLevelDir, intLevelDir, fullDir))
                                {
                                    printf ("Fatal Error... Unable to create subdirectories into the target directory\n");
                                    exit(-1);
                                }
                                if (copyTemplates(currentType, templateDir, fullDir))
                                {
                                    printf ("Fatal Error... Unable to copy templates into the target directory\n");
                                    exit(-1);
                                }
                                break;
                            } /* case TOPOLOGYTEMPTYPE */
                            default:
                            {   /* Check that no topology has been defined in the same file */
                                switch (yamlContent)
                                {
                                    case TOPOLOGY:
                                    {
                                        printf ("Fatal Error... the input yaml file cannot contain type definitions and a topology at the same time\n");
                                        exit (-1);
                                    }   /* case TYPES */
                                    default:
                                        break;
                                }   /* switch (yamlContent) */
                                /* Evaluate name, type, file, and all directories */
                                yamlContent = TYPES;
                                break;
                            }   /* default */
                        }   /* switch (currentType) */
                        break;
                    }   /* case MAINTYPE */
                    case TOSCATYPE:
                    {   /* At this level we expect the name of the TOSCA Type being defined */
                        switch (currentType)
                        {
                            case NULLTYPE:
                            {   /* We are neither in the case of a type, nor in the    */
                                /* case of a topology template. This scalar may be the */
                                /* targetNameSpace in the metadata at the top level    */
                                switch (lastKey)
                                {
                                    case NULLKEY:
                                    {
                                        if (strcmp((char*)event.data.scalar.value,TARGETNAMESPACE)==0)
                                            lastKey = TARGETNSKEY;
                                        break;
                                    }   /* case NULLKEY */
                                    case TARGETNSKEY:
                                    {
                                        strcpy (targetNameSpace,(char*)event.data.scalar.value);
                                        lastKey = NULLKEY;
                                        break;
                                    }   /* case TARGETNSKEY */
                                    default:
                                        break;
                                }   /* switch (lastKey) */
                                break;
                            }   /* case NULLTYPE */
                            case TOPOLOGYTEMPTYPE:
                            {   /* Parsing of topology_template structure                    */
                                /* This has a different handling with respect to data_types, */
                                /* node_types, etc. --> Just look for description            */
                                if (lastKey==DESCRIPTIONKEY)
                                {   /* Please observe that this overwrites a possible */
                                    /* description at the top level                   */
                                    sprintf (description,"%s",(char*)event.data.scalar.value);
                                    lastKey=NULLKEY;
                                if ( (strcmp(DESCRIPTION,(char*)event.data.scalar.value)==0) && (lastKey==NULLKEY) )
                                    /* Found description at the topology_template level */
                                    lastKey = DESCRIPTIONKEY;
                                }   /* if (lastKey==DESCRIPTIONKEY) */
                                break;
                            }   /* case TOPOLOGYTEMPTYPE */
                            default:
                            {   /* Insert in uri the tosca type read from YAML then *
                                 * extract relevant info from it                    */
                                sprintf (uri,"%s",(char*)event.data.scalar.value);
                                extractInfoFromUri (currentType, baseDir, uri, name, type, file, topLevelDir, intLevelDir, fullDir);
                                /* Reset description and derived_from clauses */
                                strcpy (description,DEFDESCRIPTION);
                                strcpy (derivedFrom,DEFDERIVEDFROM);
                                lastKey = NULLKEY;
                                /* Insert the TOSCA Type just read into the internal fmrt cache for the second pass */
                                if ( (res=fmrtCreate(TABLEID,uri,name, currentType, type, file, fullDir))!=FMRTOK)
                                {
                                    printFmrtLibError (res);
                                    exit(-1);
                                }
                                /* Now create the full directory structure and copy files from template dir */
                                if (createDirStruct(topLevelDir, intLevelDir, fullDir))
                                {
                                    printf ("Fatal Error... Unable to create subdirectories into the target directory\n");
                                    exit(-1);
                                }
                                if (copyTemplates(currentType, templateDir, fullDir))
                                {
                                    printf ("Fatal Error... Unable to copy templates into the target directory\n");
                                    exit(-1);
                                }
                                break;
                            }   /* default --> all other types */
                        }   /* switch (currentType) */
                        break;
                    }   /* case TOSCATYPE */
                    case PROPERTIES:
                    {
                        switch (currentType)
                        {
                            case NULLTYPE:  /* This type at this level is not recognized */
                                break;
                            case TOPOLOGYTEMPTYPE:
                            {   /* Parsing of topology_template structure */
                                /* This has a different handling with     */
                                /* respect to data_type, node_type, etc.  */
                                break;
                            }   /* case TOPOLOGYTEMPTYPE */
                            default:
                            {   /* At this level we expect relevant properties (description, etc.) */
                                switch (lastKey)
                                {
                                    case DERIVEDKEY:
                                    {
                                        sprintf (derivedFrom,"%s",(char*)event.data.scalar.value);
                                        lastKey = NULLKEY;
                                        break;
                                    }   /* case DERIVEDKEY */
                                    case DESCRIPTIONKEY:
                                    {
                                        sprintf (description,"%s",(char*)event.data.scalar.value);
                                        lastKey = NULLKEY;
                                        break;
                                    }   /* case DESCRIPTIONKEY */
                                    case NULLKEY:
                                    {
                                        if (strcmp((char*)event.data.scalar.value,DERIVEDFROM)==0)
                                            lastKey = DERIVEDKEY;
                                        if (strcmp((char*)event.data.scalar.value,DESCRIPTION)==0)
                                            lastKey = DESCRIPTIONKEY;
                                        break;
                                    }   /* case NULLKEY */
                                    default:
                                        break;
                                }   /* switch (lastKey) */
                                break;
                            }   /* default --> all other types */
                        }   /* switch (currentType) */
                        break;
                    }   /* case PROPERTIES */
                }   /* switch (currentLevel) */
                break;
            }   /* case YAML_SCALAR_EVENT: */
            case YAML_NO_EVENT:
            default:
            {   /* Should never happen */
                #ifdef DEBUG
                printf("Empty or unrecognized event!\n");
                #endif
                break;
            } 
        }   /* switch(event.type) */
        
        if(event.type != YAML_STREAM_END_EVENT)
            yaml_event_delete(&event);

    } while(event.type != YAML_STREAM_END_EVENT);
    yaml_event_delete(&event);

    /* ---------------------------------------- */
    /* Step 5 - Cleanup libyaml data structures */
    /* ---------------------------------------- */
    yaml_parser_delete(&parser);

    /* ------------------------------------------------------ */
    /* Step 6 - Parse again libyaml file since target *.tosca */
    /* files have not been filled yet                         */
    /* Handling is a bit tricky because we may have or not    */
    /* have top level metadata to add to the *.tosca file.    */
    /* Further, we have to cover either a yaml file containing*/
    /* multiple types or a single topology template           */
    /* ------------------------------------------------------ */
    #ifdef DEBUG
    printf("\nStarting second pass on input YAML file...\n");
    #endif
    yamlFptr = fopen (inputYamlFile,"r");
    if (yamlFptr==NULL)
    {
        printf ("Fatal Error... Not able to open input yaml file for second pass...\n");
        exit(-1);
    }
    #ifdef DEBUG
    printf("\nYAML file correctly open...\n");
    #endif
    if (topLevelMetadata==true)
    {   /* The yaml input file contains some top level metadata */
        /* Copy them into a separate file that will be appended */
        /* to the .tosca file in the target directory           */
        #ifdef DEBUG
        printf("Open the hidden file .toplevel.metadata...\n");
        #endif
        /* Open the hidden file .toplevel.metadata */
        if ( (topLevelMetadataFptr=fopen(topLevelMetadataFileName,"w"))== NULL )
        {   /* Unable to open .toplevel.metadata */
            printf ("Fatal Error... Not able to open temporary file for top level metadata...\n");
            exit(-1);
        }   /* if ( (topLevelMetadataFptr)== NULL ) */
        /* Skip all lines from the beginning until we find top level metadata */
        #ifdef DEBUG
        printf ("Skip all lines from the beginning until we find top level metadata... ");
        #endif
        while (fgets(lineString,MAXLINE,yamlFptr) )
        {
            if (strncmp (lineString,METADATA,strlen(METADATA))==0)
                break;
        }   /* while (fgets(lineString,MAXLINE,yamlFptr) ) */
        /* Copy all lines that belong to the metadata section */
        #ifdef DEBUG
        printf ("and copy them... ");
        #endif
        currentFilePos = ftell(yamlFptr);
        while (fgets(lineString,MAXLINE,yamlFptr) )
        {
            currentFilePos = ftell(yamlFptr);
            if (lineString[0] == ' ')
                fprintf (topLevelMetadataFptr,"%s",lineString);
            else
                break;            
        }   /* while (fgets(lineString,MAXLINE,yamlFptr) ) */
        fseek (yamlFptr,currentFilePos,SEEK_SET);
        fclose (topLevelMetadataFptr);
        #ifdef DEBUG
        printf ("DONE\n");
        #endif
    }   /* if (topLevelMetadata==true) */

    /* The yaml input file is still open and the current pointer  */
    /* is set to the first line after the end of metadata. Handle */
    /* the rest of the file by copying and/or splitting its data  */
    /* to the proper *.tosca files */
    switch (yamlContent)
    {
        case TYPES:
        {   /* the file contains one or more data_types, node_types, etc. */
            while (fgets(lineString,MAXLINE,yamlFptr) )
            {   /* skip all lines that do not begin with space since not interesting for us */
                if (lineString[0] != ' ')
                    continue;
                /* Go on up to the first character different from a space */
                p = lineString;
                while ( *p==' ')
                    p++;
                /* if the string is terminated skip the line and go on with the next one */
                if ( *p =='\0' )
                    continue;
                if ((q=strchr (p,':')) != NULL )
                {
                    *q = '\0';
                    if ( (res=fmrtRead (TABLEID,p,name, &currentType, type, file, fullDir)) == FMRTOK)
                    {   /* Found a new TOSCA Type while parsing yaml file */
                        if ( toscaFileName[0]!='\0')
                            fclose (toscaFptr);     /* Close the previous one (if any)... */
                        /* ... and open the new one in append mode */
                        sprintf (toscaFileName,"%s/%s",fullDir,toscaFile[currentType]);
                        if (topLevelMetadata==true)     /* append top level metadata from .toplevel.metadata file into .tosca file */
                            appendMetaData (toscaFileName,topLevelMetadataFileName);
                        if ( (toscaFptr=fopen (toscaFileName,"a")) == NULL)
                        {
                            printf ("Fatal Error... Unable to open Tosca File %s\n",toscaFileName);
                            exit (-1);
                        }
                        fprintf (toscaFptr,"\n%s:\n",yamlMatchString[currentType]);
                    }   /* if ( (res=fmrtRead (TABLEID,p,name, currentType, type, file, fullDir)) == FMRTOK) */
                    *q = ':';
                }   /* if ((q=strchr (p,':')) != NULL ) */
                if (toscaFptr)
                    fprintf (toscaFptr,"%s",lineString);
            }   /* while (fgets(lineString,MAXLINE,yamlFptr) ) */
            break;
        }   /* case TYPES */
        case TOPOLOGY:
        {   /* the file contains one and just one topology template */
            if ( (res=fmrtRead (TABLEID,yamlMatchString[TOPOLOGYTEMPTYPE],name, &currentType, type, file, fullDir)) != FMRTOK)
            {   /* This should never happen since if we are here the internal table */
                /* contains just an entry with key topology_template, but who knows */
                printf ("Fatal Error... Corrupted data\n");
                exit (-1);
            }   /* if ( (res=fmrtRead (TABLEID,p,name, &currentType, type, file, fullDir)) != FMRTOK) */
            sprintf (toscaFileName,"%s/%s",fullDir,toscaFile[currentType]);
            if (topLevelMetadata==true)     /* append top level metadata from .toplevel.metadata file into .tosca file */
                appendMetaData (toscaFileName,topLevelMetadataFileName);
            if ( (toscaFptr=fopen (toscaFileName,"a")) == NULL)
            {
                printf ("Fatal Error... Unable to open Tosca File %s\n",toscaFileName);
                exit (-1);
            }
            fprintf (toscaFptr,"\n%s:\n",yamlMatchString[currentType]);
            /* Skip all lines from the beginning until we find topology_template string */
            while (fgets(lineString,MAXLINE,yamlFptr) )
            {
                if (strncmp (lineString,TOPYAMLMATCH,strlen(TOPYAMLMATCH))==0)
                    break;
            }   /* while (fgets(lineString,MAXLINE,yamlFptr) ) */
            /* Now copy all the rest of the lines from input file to *.tosca file in the target directory */
            /* Be aware that the input yaml file cannot contain other stuff besides the topology template */
            while (fgets(lineString,MAXLINE,yamlFptr) )
                fprintf (toscaFptr,"%s",lineString);
            fclose (toscaFptr);
            break;
        }   /* case TOPOLOGY */
        default:
        {  
            printf ("The input YAML file does neither contain a topology, not type definitions\n");
            break;
        }   /* default */
    }   /* switch (yamlContent) */

    /* The yaml input file has reached the end */
    #ifdef DEBUG
    printf("YAML file correctly handled...\n");
    #endif
    /* Close everything and exit               */
    if ( toscaFileName[0]=='\0')
        fclose (toscaFptr);
    fclose (yamlFptr);
    if (topLevelMetadata==true)
        remove (topLevelMetadataFileName);
    fmrtClearTable(TABLEID);

    exit (0);
}