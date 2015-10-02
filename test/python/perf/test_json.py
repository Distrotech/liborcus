#!/usr/bin/env python3

from datetime import datetime
import json
import orcus.json

swagger_json = """
{
    "swagger": "2.0",
    "info": {
        "version": "1.0.0",
        "title": "Swagger Petstore (Simple)",
        "description": "A sample API that uses a petstore as an example to demonstrate features in the swagger-2.0 specification",
        "termsOfService": "http://helloreverb.com/terms/",
        "contact": {
            "name": "Swagger API team",
            "email": "foo@example.com",
            "url": "http://swagger.io"
        },
        "license": {
            "name": "MIT",
            "url": "http://opensource.org/licenses/MIT"
        }
    },
    "host": "petstore.swagger.wordnik.com",
    "basePath": "/api",
    "schemes": [
        "http"
    ],
    "consumes": [
        "application/json"
    ],
    "produces": [
        "application/json"
    ],
    "paths": {
        "/pets": {
            "get": {
                "description": "Returns all pets from the system that the user has access to",
                "operationId": "findPets",
                "produces": [
                    "application/json",
                    "application/xml",
                    "text/xml",
                    "text/html"
                ],
                "parameters": [
                    {
                        "name": "tags",
                        "in": "query",
                        "description": "tags to filter by",
                        "required": false,
                        "type": "array",
                        "items": {
                            "type": "string"
                        },
                        "collectionFormat": "csv"
                    },
                    {
                        "name": "limit",
                        "in": "query",
                        "description": "maximum number of results to return",
                        "required": false,
                        "type": "integer",
                        "format": "int32"
                    }
                ],
                "responses": {
                    "200": {
                        "description": "pet response",
                        "schema": {
                            "type": "array",
                            "items": {
                                "$ref": "#/definitions/pet"
                            }
                        }
                    },
                    "default": {
                        "description": "unexpected error",
                        "schema": {
                            "$ref": "#/definitions/errorModel"
                        }
                    }
                }
            },
            "post": {
                "description": "Creates a new pet in the store.  Duplicates are allowed",
                "operationId": "addPet",
                "produces": [
                    "application/json"
                ],
                "parameters": [
                    {
                        "name": "pet",
                        "in": "body",
                        "description": "Pet to add to the store",
                        "required": true,
                        "schema": {
                            "$ref": "#/definitions/newPet"
                        }
                    }
                ],
                "responses": {
                    "200": {
                        "description": "pet response",
                        "schema": {
                            "$ref": "#/definitions/pet"
                        }
                    },
                    "default": {
                        "description": "unexpected error",
                        "schema": {
                            "$ref": "#/definitions/errorModel"
                        }
                    }
                }
            }
        },
        "/pets/{id}": {
            "get": {
                "description": "Returns a user based on a single ID, if the user does not have access to the pet",
                "operationId": "findPetById",
                "produces": [
                    "application/json",
                    "application/xml",
                    "text/xml",
                    "text/html"
                ],
                "parameters": [
                    {
                        "name": "id",
                        "in": "path",
                        "description": "ID of pet to fetch",
                        "required": true,
                        "type": "integer",
                        "format": "int64"
                    }
                ],
                "responses": {
                    "200": {
                        "description": "pet response",
                        "schema": {
                            "$ref": "#/definitions/pet"
                        }
                    },
                    "default": {
                        "description": "unexpected error",
                        "schema": {
                            "$ref": "#/definitions/errorModel"
                        }
                    }
                }
            },
            "delete": {
                "description": "deletes a single pet based on the ID supplied",
                "operationId": "deletePet",
                "parameters": [
                    {
                        "name": "id",
                        "in": "path",
                        "description": "ID of pet to delete",
                        "required": true,
                        "type": "integer",
                        "format": "int64"
                    }
                ],
                "responses": {
                    "204": {
                        "description": "pet deleted"
                    },
                    "default": {
                        "description": "unexpected error",
                        "schema": {
                            "$ref": "#/definitions/errorModel"
                        }
                    }
                }
            }
        }
    },
    "definitions": {
        "pet": {
            "required": [
                "id",
                "name"
            ],
            "properties": {
                "id": {
                    "type": "integer",
                    "format": "int64"
                },
                "name": {
                    "type": "string"
                },
                "tag": {
                    "type": "string"
                }
            }
        },
        "newPet": {
            "required": [
                "name"
            ],
            "properties": {
                "id": {
                    "type": "integer",
                    "format": "int64"
                },
                "name": {
                    "type": "string"
                },
                "tag": {
                    "type": "string"
                }
            }
        },
        "errorModel": {
            "required": [
                "code",
                "message"
            ],
            "properties": {
                "code": {
                    "type": "integer",
                    "format": "int32"
                },
                "message": {
                    "type": "string"
                }
            }
        }
    }
}
"""

class ScopePrinter(object):

    def __init__(self, name):
        self.start_time = None
        self.name = name

    def __enter__(self):
        self.start_time = datetime.now()
        print("--- {} started".format(self.name))

    def __exit__(self, type, value, traceback):
        end_time = datetime.now()
        diff = end_time - self.start_time
        sec = diff.seconds + diff.microseconds / 1000000.0
        print("--- {} ended (duration: {} seconds)".format(self.name, sec))


def run_builtin():
    for i in range(20000):
        o = json.loads(swagger_json)


def run_orcus():
    for i in range(20000):
        o = orcus.json.loads(swagger_json)


def main():
    with ScopePrinter("builtin"):
        run_builtin()

    with ScopePrinter("orcus"):
        run_orcus()


if __name__ == '__main__':
    main()

