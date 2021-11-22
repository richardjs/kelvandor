terraform {
  required_providers {
    docker = {
      source = "kreuzwerker/docker"
    }
  }
}

provider "docker" {}

resource "docker_image" "api" {
  name = "api"
  build {
    path = "."
  }
}

resource "docker_container" "api" {
  name  = "api"
  image = docker_image.api.latest
  ports {
    internal = 5000
    external = 5000
  }
}

resource "docker_image" "nginx" {
  name = "nginx"
}

resource "docker_container" "nginx" {
  name  = "nginx"
  image = docker_image.nginx.latest
  ports {
    internal = 80
    external = 8000
  }
  volumes {
    host_path      = "${abspath(path.root)}/html"
    container_path = "/usr/share/nginx/html"
    read_only      = true
  }
}
