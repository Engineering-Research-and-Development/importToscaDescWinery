#######################################################################################
#                                                                                     #
# checkReadMe.sh:   This shell script parses all README.md files looking for some     #
#                   placeholders not correctly updated, and provides the list of      #
#                   files that require manual intervention                            #
#                   It shall be invoked from the directory hosting the repository     #
# Author:           Roberto Mameli                                                    #
# Version:          v.1.0.0                                                           #
# Date:             September 2023                                                    #
#                                                                                     #
# ----------------------------------------------------------------------------------- #
#                                                                                     #
# Web:       https://www.eng.it/                                                      #
# Linkedin:  https://www.linkedin.com/company/engineering-ingegneria-informatica-spa/ #
# Twitter:   https://twitter.com/EngineeringSpa                                       #
# Instagram: https://www.instagram.com/lifeatengineering/                             #
#                                                                                     #
# This contribution has been realized by Engineering Ingegneria Informatica S.p.A     #
# and is available for free use under the terms of the Apache v.2.0 license (see      #
# below)                                                                              #
#                                                                                     #
# ----------------------------------------------------------------------------------- #
#                                                                                     #
# Copyright 2023 Engineering Ingegneria Informatica S.p.A.                            #
#                                                                                     #
# Licensed under the Apache License, Version 2.0 (the "License");                     #
# you may not use this file except in compliance with the License.                    #
# You may obtain a copy of the License at                                             #
#                                                                                     #
#     http://www.apache.org/licenses/LICENSE-2.0                                      #
#                                                                                     #
# Unless required by applicable law or agreed to in writing, software                 #
# distributed under the License is distributed on an "AS IS" BASIS,                   #
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.            #
# See the License for the specific language governing permissions and                 #
# limitations under the License.                                                      #
#                                                                                     #
#######################################################################################
#!/bin/bash


# -------------------------------------------------------
# Check if some README.md files was not correctly updated
# -------------------------------------------------------
echo
read -p "Do you want to check README.md files? " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]
then
    echo "Here follows the list of README.files that have not been correctly processed"
    echo "Please check them manually...."
    grep -l -r -I -i "\$1" ./artifacttypes | grep README
    grep -l -r -I -i "\$1" ./capabilitytypes | grep README
    grep -l -r -I -i "\$1" ./relationshiptypes | grep README
    grep -l -r -I -i "\$1" ./interfacetypes | grep README
    grep -l -r -I -i "\$1" ./grouptypes | grep README
    grep -l -r -I -i "\$1" ./policytypes | grep README
    grep -l -r -I -i "\$1" ./datatypes | grep README
    grep -l -r -I -i "\$1" ./nodetypes | grep README
    grep -l -r -I -i "\$1" ./servicetemplates | grep README
fi


