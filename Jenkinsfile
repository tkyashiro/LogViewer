node("")
{
	stage("CMake")
	{
		echo "cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo"
	}

	stage("Build")
	{
		echo "cmake --build . --target clean"
		echo "cmake --build . --target all"
	}

	stage("Test")
	{
		echo "cmake --build . --target test"
	}
}
