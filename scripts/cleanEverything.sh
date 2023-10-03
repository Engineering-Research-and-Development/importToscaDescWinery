#######################################################################################
#                                                                                     #
# cleanEverything.sh: This shell script removes all the content of a repository       #
#                     It shall be invoked from the directory hosting the repository   #
# Author:             Roberto Mameli                                                  #
# Version:            v.1.0.0                                                         #
# Date:               September 2023                                                  #
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


# ---------------------------------
# Remove all the repository content
# ---------------------------------
echo
echo "You asked to remove all the repository content"
echo "After this command the repository will be left empty"
echo "This operation cannot be recovered"
read -p "Are you really sure? " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]
then
    echo "Removing all the repository content"
    echo
    rm -rf ./artifacttypes
    rm -rf ./capabilitytypes
    rm -rf ./datatypes
    rm -rf ./grouptypes
    rm -rf ./interfacetypes
    rm -rf ./nodetypes
    rm -rf ./policytypes
    rm -rf ./relationshiptypes
    rm -rf ./servicetemplates
    rm -rf ./admin
    echo "Done"
    exit 0;
fi

echo "OK.... skipping"
exit 0;
