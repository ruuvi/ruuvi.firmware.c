// Jenkinsfile (Declarative Pipeline)
pipeline {
    agent any

    stages {
        stage('Build') {
            steps {
                echo 'Building..'
                dir('src'){
                sh 'make clean'
                sh 'make sync'
                sh 'make'
                archiveArtifacts artifacts: '**/targets/ruuvitag_b/armgcc/*.hex, **/targets/ruuvitag_b/armgcc/*.zip', fingerprint: true
                }
            }
        }
        stage('Test') {
            steps {
                echo 'Testing..'
                ceedling
            }
        }
        stage('Deploy') {
            steps {
                echo 'Deploying....'
            }
        }
    }
}