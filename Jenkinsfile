stage('Build') {
	parallel linux: {
		node('linux') {
			checkout scm
			sh 'git submodule update --init --recursive'
			sh 'mkdir -p build && cd build && python ../support/jenkins/buildAllModules.py && make'
		}
	},
	windows: {
		node('windows') {
			checkout scm
			bat 'git submodule update --init --recursive'
			bat 'mkdir -p build && cd build && python ../support/jenkins/buildAllModules.py && make'
		}
	}
	mac: {
		node('osx') {
			checkout scm
			sh 'git submodule update --init --recursive'
			sh 'mkdir -p build && cd build && python ../support/jenkins/buildAllModules.py && make'
		}
	}
}