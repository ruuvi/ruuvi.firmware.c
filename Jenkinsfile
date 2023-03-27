// Jenkinsfile (Declarative Pipeline)
pipeline {
    agent any
    environment {
        GH_TOKEN = credentials('jenkins-github-token')
    }
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
                sh 'ceedling'
            }
        }
        stage('Deploy') {
            when {
                buildingTag()
            }
            steps {
                echo 'Deploying....'            
                dir('src'){
                sh 'release.sh'
                }
            }
        }
    }
}