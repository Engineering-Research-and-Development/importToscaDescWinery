/***************************************************************************************
 *                                                                                     *
 * importToscaDescWinery.h: This application parses a tosca descriptor in yaml format  *
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

#ifndef ITDW_H_
#define ITDW_H_

#ifdef __cplusplus
extern "C" {
#endif


/*********************
 * Macro Definitions *
 *********************/
//#define DEBUG                                /* Uncomment this to get debug printouts */

#define TABLEID                            0   /* Arbitrary value between 0 and 255 */
#define TABLENAME                   "CSVData"  /* Table used for storing input CSV Data */
#define NUMFIELDS                          5   /* Number of fields in the table */

#define URINAME                         "URI"  /* Name of the key (Full TOSCA Type URI) */
#define URILENGTH                        128   /* Max length of Full TOSCA Type (URI) */
#define NAMENAME                       "Name"  /* Name of the field TOSCA Type Name */
#define NAMELENGTH                        64   /* Max length of TOSCA Type Name */
#define TYPEINTNAME                 "TypeInt"  /* Integer corresponding to the field TOSCA Type */
#define TYPENAME                       "Type"  /* Name of the field TOSCA Type */
#define TYPELENGTH                        16   /* Max length of TOSCA Type */
#define FILENAME                       "File"  /* Name of the field .tosca file name */
#define FILELENGTH                        32   /* Max length of .tosca file name */
#define DIRNAME                     "FullDir"  /* Name of the field full directory */
#define DIRLENGTH                        254   /* Max length of full directory (limit due to current libfmrt version )*/

#define MAXYAMLTYPES                    5000   /* Max number of types from yaml file (arbitrary choice) */

#define DEFBASEDIR                       "./"  /* Default Base Directory */
#define DEFAULTMODE                     0777   /* Default mode for directories that are created */

#define MAXLINE                         2047   /* Max length of a single file line */
#define MAXSTRING                         64   /* Max string length */
#define MAXTYPES                           9   /* Number of TOSCA types */
#define MAXTOKEN                          10   /* Max number of tokens in the Tosca Type */

#define METADATA                   "metadata"  /* Match string into yaml file */
#define DERIVEDFROM            "derived_from"  /* Match string into yaml file */
#define DESCRIPTION             "description"  /* Match string into yaml file */
#define TARGETNAMESPACE     "targetNamespace"  /* Match string into yaml file */
#define DEFDERIVEDFROM                  "N/A"  /* Default Value of tag derived_from */
#define DEFDESCRIPTION                  "N/A"  /* Default Value of tag description */
#define DEFTARGETNS "example.org.tosca.servicetemplates" /* Default Value of targetNamespace */

#define NULLKEY                            0   /* Used for reading description, derived from and targetNameSpace */
#define DERIVEDKEY                         1   /* Used for reading description, derived from and targetNameSpace */
#define DESCRIPTIONKEY                     2   /* Used for reading description, derived from and targetNameSpace */
#define TARGETNSKEY                        3   /* Used for reading description, derived from and targetNameSpace */

#define ARTIFACTTYPE                       0   /* Id for Artifact type */
#define CAPABILITYTYPE                     1   /* Id for Capability type */
#define DATATYPE                           2   /* Id for Data type */
#define GROUPTYPE                          3   /* Id for Group type */
#define INTERFACETYPE                      4   /* Id for Interface type */
#define NODETYPE                           5   /* Id for Node type */
#define POLICYTYPE                         6   /* Id for Policy type */
#define RELATIONSHIPTYPE                   7   /* Id for Relationship type */
#define TOPOLOGYTEMPTYPE                   8   /* Id for Topology Template */
#define NULLTYPE                          -1   /* Id for Null Type */

#define ARTYAMLMATCH         "artifact_types"  /* Match string for Artifact type */
#define CAPYAMLMATCH       "capability_types"  /* Match string for Capability type */
#define DATYAMLMATCH             "data_types"  /* Match string for Data type */
#define GROYAMLMATCH            "group_types"  /* Match string for Group type */
#define INTYAMLMATCH        "interface_types"  /* Match string for Interface type */
#define NODYAMLMATCH             "node_types"  /* Match string for Node type */
#define POLYAMLMATCH           "policy_types"  /* Match string for Policy type */
#define RELYAMLMATCH     "relationship_types"  /* Match string for Relationship type */
#define TOPYAMLMATCH      "topology_template"  /* Match string fot Topology Template */

#define ARTTYPE                    "Artifact"  /* Artifact type */
#define CAPTYPE                  "Capability"  /* Capability type */
#define DATTYPE                        "Data"  /* Data type */
#define GROTYPE                       "Group"  /* Group type */
#define INTTYPE                   "Interface"  /* Interface type */
#define NODTYPE                        "Node"  /* Node type */
#define POLTYPE                      "Policy"  /* Policy type */
#define RELTYPE                "Relationship"  /* Relationship type */
#define TOPTYPE                     "Service"  /* Topology Template */

#define ARTDIR               "artifacttypes/"  /* Relative Top Directory for Artifact type */
#define CAPDIR             "capabilitytypes/"  /* Relative Top Directory for Capability type */
#define DATDIR                   "datatypes/"  /* Relative Top Directory for Data type */
#define GRODIR                  "grouptypes/"  /* Relative Top Directory for Group type */
#define INTDIR              "interfacetypes/"  /* Relative Top Directory for Interface type */
#define NODDIR                   "nodetypes/"  /* Relative Top Directory for Node type */
#define POLDIR                 "policytypes/"  /* Relative Top Directory for Policy type */
#define RELDIR           "relationshiptypes/"  /* Relative Top Directory for Relationship type */
#define TOPDIR            "servicetemplates/"  /* Relative Top Directory for Topology Templates */

#define ARTTOSCAFILE     "ArtifactType.tosca"  /* tosca file template for Artifact type */
#define CAPTOSCAFILE   "CapabilityType.tosca"  /* tosca file template for Capability type */
#define DATTOSCAFILE         "DataType.tosca"  /* tosca file template for Data type */
#define GROTOSCAFILE        "GroupType.tosca"  /* tosca file template for Group type */
#define INTTOSCAFILE    "InterfaceType.tosca"  /* tosca file template for Interface type */
#define NODTOSCAFILE         "NodeType.tosca"  /* tosca file template for Node type */
#define POLTOSCAFILE       "PolicyType.tosca"  /* tosca file template for Policy type */
#define RELTOSCAFILE "RelationshipType.tosca"  /* tosca file template for Relationship type */
#define TOPTOSCAFILE  "ServiceTemplate.tosca"  /* tosca file template for Topology Templates */

#define DOCUMENT                            0  /* Root Level in the YAML file (i.e. document) */
#define MAINTYPE                            1  /* First Level (data_types, node_types, etc.) */
#define TOSCATYPE                           2  /* Second Level for TOSCA Type under definition */
#define PROPERTIES                          3  /* Values for the properties associated to TOSCA Type */
                                               /* (higher levels are not needed) */

#define UNKNOWN                             0  /* Initial value for yamlContent indicator             */
#define TYPES                               1  /* Assigned to yamlContent when yaml contains types    */
#define TOPOLOGY                            2  /* Assigned to yamlContent when yaml contains topology */


/********************
 * Type Definitions *
 ********************/
typedef char    fullToscaTypeString[URILENGTH+1];
typedef char    typeNameString[NAMELENGTH+1];
typedef char    typeString[TYPELENGTH+1];
typedef char    shortFileNameString[FILELENGTH+1];
typedef char    fullFileNameString[DIRLENGTH+1];
typedef char    shortString[MAXSTRING+1];
typedef char    longString[MAXLINE+1];


/************************
 * Function Definitions *
 ************************/
/* Functions implemented in importToscaUtils.c */
void          printUsage (void);
void          printFmrtLibError(fmrtResult);
/* Functions implemented in importToscaYaml.c */
void          fillReadMe (FILE *, FILE *, char *, char *, char *, char *, char *);
int           createDirStruct(char *, char *, char *);
int           copyTemplates(int , char *, char *);
void          extractInfoFromUri(int , char *, char *, char *, char *, char *, char *, char *, char *);
int           extractTopologyTemplateName(char *, char *);
int           appendMetaData (char *, char *);

#ifdef __cplusplus
} // end extern "C"
#endif

#endif /* ITDW_H_ */