##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=netcap
ConfigurationName      :=Debug
WorkspacePath          :=/home/zooba/works/netcap
ProjectPath            :=/home/zooba/works/netcap
IntermediateDirectory  :=./build
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=zooba
Date                   :=17/10/16
CodeLitePath           :=/home/zooba/.codelite
LinkerName             :=g++
SharedObjectLinkerName :=g++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.o.i
DebugSwitch            :=-gstab
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=$(PreprocessorSwitch)DEBUG $(PreprocessorSwitch)_LINUX 
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E 
ObjectsFileList        :="netcap.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  -linotifytools -lopencv_stitching -lopencv_superres -lopencv_videostab  -lopencv_video  -lopencv_calib3d -lopencv_features2d -lopencv_flann  -lopencv_objdetect -lopencv_ml  -lopencv_highgui  -lopencv_photo -lopencv_imgproc -lopencv_core 
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). 
IncludePCH             := 
RcIncludePath          := 
Libs                   := 
ArLibs                 :=  
LibPath                := $(LibraryPathSwitch). $(LibraryPathSwitch)/usr/local/lib $(LibraryPathSwitch)/usr/local/lib 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := ar rcus
CXX      := g++
CC       := gcc
CXXFLAGS :=  -g -O0 -Wall -std=c++11 $(Preprocessors)
CFLAGS   :=  -g -O0 -Wall  $(Preprocessors)
ASFLAGS  := 
AS       := as


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects0=$(IntermediateDirectory)/src_client.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_main.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_netcap_common.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_server.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_client_opencv.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_server_opencv.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

MakeIntermediateDirs:
	@test -d ./build || $(MakeDirCommand) ./build


$(IntermediateDirectory)/.d:
	@test -d ./build || $(MakeDirCommand) ./build

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/src_client.cpp$(ObjectSuffix): src/client.cpp $(IntermediateDirectory)/src_client.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/zooba/works/netcap/src/client.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_client.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_client.cpp$(DependSuffix): src/client.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_client.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_client.cpp$(DependSuffix) -MM src/client.cpp

$(IntermediateDirectory)/src_client.cpp$(PreprocessSuffix): src/client.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_client.cpp$(PreprocessSuffix)src/client.cpp

$(IntermediateDirectory)/src_main.cpp$(ObjectSuffix): src/main.cpp $(IntermediateDirectory)/src_main.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/zooba/works/netcap/src/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_main.cpp$(DependSuffix): src/main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_main.cpp$(DependSuffix) -MM src/main.cpp

$(IntermediateDirectory)/src_main.cpp$(PreprocessSuffix): src/main.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_main.cpp$(PreprocessSuffix)src/main.cpp

$(IntermediateDirectory)/src_netcap_common.cpp$(ObjectSuffix): src/netcap_common.cpp $(IntermediateDirectory)/src_netcap_common.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/zooba/works/netcap/src/netcap_common.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_netcap_common.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_netcap_common.cpp$(DependSuffix): src/netcap_common.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_netcap_common.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_netcap_common.cpp$(DependSuffix) -MM src/netcap_common.cpp

$(IntermediateDirectory)/src_netcap_common.cpp$(PreprocessSuffix): src/netcap_common.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_netcap_common.cpp$(PreprocessSuffix)src/netcap_common.cpp

$(IntermediateDirectory)/src_server.cpp$(ObjectSuffix): src/server.cpp $(IntermediateDirectory)/src_server.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/zooba/works/netcap/src/server.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_server.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_server.cpp$(DependSuffix): src/server.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_server.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_server.cpp$(DependSuffix) -MM src/server.cpp

$(IntermediateDirectory)/src_server.cpp$(PreprocessSuffix): src/server.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_server.cpp$(PreprocessSuffix)src/server.cpp

$(IntermediateDirectory)/src_client_opencv.cpp$(ObjectSuffix): src/client_opencv.cpp $(IntermediateDirectory)/src_client_opencv.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/zooba/works/netcap/src/client_opencv.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_client_opencv.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_client_opencv.cpp$(DependSuffix): src/client_opencv.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_client_opencv.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_client_opencv.cpp$(DependSuffix) -MM src/client_opencv.cpp

$(IntermediateDirectory)/src_client_opencv.cpp$(PreprocessSuffix): src/client_opencv.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_client_opencv.cpp$(PreprocessSuffix)src/client_opencv.cpp

$(IntermediateDirectory)/src_server_opencv.cpp$(ObjectSuffix): src/server_opencv.cpp $(IntermediateDirectory)/src_server_opencv.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/zooba/works/netcap/src/server_opencv.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_server_opencv.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_server_opencv.cpp$(DependSuffix): src/server_opencv.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_server_opencv.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_server_opencv.cpp$(DependSuffix) -MM src/server_opencv.cpp

$(IntermediateDirectory)/src_server_opencv.cpp$(PreprocessSuffix): src/server_opencv.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_server_opencv.cpp$(PreprocessSuffix)src/server_opencv.cpp


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./build/


