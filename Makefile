#######################################################################################
#                                                                                     #
# importToscaDescWinery:   This application parses a tosca descriptor in yaml format  #
#                          and organizes information contained therein into a         #
#                          directory suitable for Eclipse Winery                      #
# Author:                  Roberto Mameli                                             #
# Version:                 v.1.0.0                                                    #
# Date:                    September 2023                                             #
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

BIN = ./bin
CC = gcc
RM = rm
CP = cp
MV = mv

INCLUDE = -I./include
OBJS = ./obj/importToscaDescWinery.o ./obj/importToscaYaml.o ./obj/importToscaUtils.o
TARGET = ./bin/importToscaDescWinery
SCRIPTS = ./scripts/checkReadMe.sh ./scripts/cleanEverything.sh
LIBRARY = -L./lib
LIBS = -lyaml -lfmrt
CFLAGS = -v -Wall
INSTALL = /usr/local/bin/
DELIVERY = /usr/local/bin/importToscaDescWinery /usr/local/bin/checkReadMe.sh /usr/local/bin/cleanEverything.sh

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $(INCLUDE) -o $@ $<

all: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(LIBRARY) $(OBJS) $(LIBS)

install:
	$(CP) -p $(TARGET) $(INSTALL)
	$(CP) -p $(SCRIPTS) $(INSTALL)

clean:
	$(RM) $(OBJS)
	$(RM) $(TARGET)

cleaninstall:
	$(RM) $(DELIVERY)

cleanall: clean cleaninstall
