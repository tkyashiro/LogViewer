node("")
{
	stage("CMake")
	{
		cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo
	}

	stage("Build")
	{
		cmake --build . --target clean
		cmake --build . --target all
	}

	stage("Test")
	{
		cmake --build . --target test
	}
}
