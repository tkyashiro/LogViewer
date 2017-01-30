node
{
	stage("Checkout")
	{
		git url: 'https://github.com/tkyashiro/LogViewer.git'
	}

	stage("CMake")
	{
		sh "mkdir -p build && cd build"
		sh "cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ../"
	}

	stage("Build")
	{
		sh "cmake --build . --target clean"
		sh "cmake --build . --target all"
	}

	stage("Test")
	{
		sh "cmake --build . --target test"
	}

	stage("Pack")
	{
		archiveArtifact artifacts: 'build/bin/*'
	}
}
