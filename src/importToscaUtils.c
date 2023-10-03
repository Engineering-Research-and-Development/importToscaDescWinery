/***************************************************************************************
 *                                                                                     *
 * importToscaUtils.c: This file contains some utilities used by importToscaDescWinery *
 * Author              Roberto Mameli                                                  *
 * Version             v.1.0.0                                                         *
 * Date                September 2023                                                  *
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
#include <sys/types.h>
#include "fmrt.h"


/*********************************************
 * Print out the command format (short form) *
 *********************************************/
void printUsage (void)
{
    printf ("Usage:\n\n");
    printf ("    importToscaDescWinery -i <yaml_tosca_descriptor> -t <template_dir> [-o <winery_base_dir>]\n\n");
    printf ("This command takes a TOSCA descriptor in YAML format (option -i) and a directory with some template\n");
    printf ("files (option -t), and provides a directory structure that can be used as repository for Eclipse\n");
    printf ("Winery, containing all types defined into the YAML descriptor (nodes, capabilities, relationships,\n");
    printf ("etc.). The output directory must exist (i.e. it is not created by the tool) and is specified through\n");
    printf ("the optional argument specified by flag -o (if not specified, current directory ./ is assumed by default.\n\n");

    return;
}


/***************************
 * Used to print the error *
 ***************************/
void printFmrtLibError(fmrtResult result)
{
    switch (result)
    {
        case FMRTOK:
        {
            printf ("Operation Succeeded\n");
            break;
        }
        case FMRTKO:
        {
            printf ("Fatal Error... Operation failed\n");
            break;
        }
        case FMRTIDALREADYEXISTS:
        {
            printf ("Fatal Error... TableId already defined\n");
            break;
        }
        case FMRTIDNOTFOUND:
        {
            printf ("Fatal Error... TableId not found\n");
            break;
        }
        case FMRTMAXTABLEREACHED:
        {
            printf ("Fatal Error... Max number of tables reached\n");
            break;
        }
        case FMRTMAXFIELDSINVALID:
        {
            printf ("Fatal Error... Specified number of fields is outside the allowed range\n");
            break;
        }
        case FMRTREDEFPROHIBITED:
        {
            printf ("Fatal Error... Key and/or Field Redefinition Prohibited\n");
            break;
        }
        case FMRTDUPLICATEKEY:
        {
            printf ("Fatal Error... The specified TOSCA Type already exists\n");
            break;
        }
        case FMRTNOTEMPTY:
        {
            printf ("Fatal Error... Table is not empty\n");
            break;
        }
        case FMRTNOTFOUND:
        {
            printf ("Fatal Error... Tosca Type has not been found\n");
            break;
        }
        case FMRTFIELDTOOLONG:
        {
            printf ("Fatal Error... String too long\n");
            break;
        }
        case FMRTOUTOFMEMORY:
        {
            printf ("Fatal Error... No more space in table\n");
            break;
        }
        default:
        {
            printf ("Fatal Error... Unrecognized error\n");
            break;
        }
    }   /* switch (result) */

    return;
}