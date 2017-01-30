node("")
{
	stage("CMake")
	{
		bat "cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo"
	}

	stage("Build")
	{
		bat "cmake --build . --target clean"
		bat "cmake --build . --target all"
	}

	stage("Test")
	{
		bat "cmake --build . --target test"
	}
}
