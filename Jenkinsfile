node
{
	stage("Checkout")
	{
		git url: 'https://github.com/tkyashiro/LogViewer.git'
	}

	stage("CMake")
	{
		sh "mkdir -p build"
		sh "cd build && cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ../"
	}

	stage("Build")
	{
		sh "cd build && cmake --build . --target clean && cmake --build . --target all"
	}

	stage("Test")
	{
		sh "cd build && rm -rf ./Testnig/ && ctest --no-compress-output -T Test || /usr/bin/true"
		junit 'build/Testing/**/Test.xml'
	}

	stage("Pack")
	{
		archiveArtifacts artifacts: 'build/bin/*'
	}
}
