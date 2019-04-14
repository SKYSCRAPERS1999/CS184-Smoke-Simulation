# DO NOT EDIT
# This makefile makes sure all linkable targets are
# up-to-date with anything they link to
default:
	echo "Do not invoke directly"

# Rules to remove targets that are older than anything to which they
# link.  This forces Xcode to relink the targets from scratch.  It
# does not seem to check these dependencies itself.
PostBuild.CGL.Debug:
/Users/ericying/cs184/Smoke-Simulation/CGL/src/Debug/libCGL.a:
	/bin/rm -f /Users/ericying/cs184/Smoke-Simulation/CGL/src/Debug/libCGL.a


PostBuild.CGL.Release:
/Users/ericying/cs184/Smoke-Simulation/CGL/src/Release/libCGL.a:
	/bin/rm -f /Users/ericying/cs184/Smoke-Simulation/CGL/src/Release/libCGL.a


PostBuild.CGL.MinSizeRel:
/Users/ericying/cs184/Smoke-Simulation/CGL/src/MinSizeRel/libCGL.a:
	/bin/rm -f /Users/ericying/cs184/Smoke-Simulation/CGL/src/MinSizeRel/libCGL.a


PostBuild.CGL.RelWithDebInfo:
/Users/ericying/cs184/Smoke-Simulation/CGL/src/RelWithDebInfo/libCGL.a:
	/bin/rm -f /Users/ericying/cs184/Smoke-Simulation/CGL/src/RelWithDebInfo/libCGL.a




# For each target create a dummy ruleso the target does not have to exist
