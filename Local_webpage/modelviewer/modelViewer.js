//File containing all functions for the modelViewer

//<VARIABLES>//

var modelviewerpath = "modelviewer/";

//debugmode parameters:
var debugMode = false;
var holding = false;
var keyCode = 0;
var cameraMovementSpeed = 10;
var cameraRotateSpeed = 80;
var pause = false;

//program parameters:
var gl;
var canvas;
var program; //gl program

//Array containg all models
var modelsArray = [];

//Array containing all lightSources
var lightSourcesArray = [];

//Array containg all textfields
var textArray = [];

//Properties of the grass:
var grassPlaneWidth = 16;
var grassPlaneHeight = 32;
var grassDensity = 0.5; // density 0-100; 100-> super dense
var grassField = new grass();

//the camera
var cameraViewer = new camera();

//Color used as transparent in textures for 2D canvas
var transparentColor = "rgb(25,255,0)";

//Parameters of 'manual' rotation of model
var xScrollIntensity = 0.01;
var yScrollIntensity = 0.01;
var dragging = false; //dragging state

//identity matrix
var identityMatrix = new Float32Array(16);
mat4.identity(identityMatrix);

//used for calculating the delta time
var then = 0;

//</VARIABLES>//

//<EVENT LISTNERS>//

//add event listners for keypresses:
window.onkeydown = function (e) {
    keyCode = e.keyCode ? e.keyCode : e.which; //save the keycode pressed
    holding = true; //used to measure a key-hold
    if (!pause && keyCode == 80) { //p pressed and it is not paused
        pause = true;
    } else if (pause && keyCode == 80) {
        pause = false;
        requestAnimationFrame(renderLoop);
    }
};

window.onkeyup = function (e) {
    holding = false;
}

//</EVENT LISTNERS>

//<FUNCTIONS>//

function startWebGL() {

    //modelsArray = models;
    //textArray = textFields

    //setting view and projmatrix in the camera
    mat4.lookAt(cameraViewer.viewMatrix, [0, 0, -20], [0, 0, 0], [0, 1, 0]);
    mat4.perspective(cameraViewer.projMatrix, (45 / 360) * 2 * Math.PI, canvas.width / canvas.height, 0.1, 1000.0);

    //create the grass
    //createGrass("rgba(43, 255, 0, 1.0)", function () {

    //load shaders
    loadShaders(function (vsText, fsText) {
        //execute when shaders are loaded


        program = compileShaders(vsText, fsText); ///compile shaders
        requestAnimationFrame(renderLoop); //start the rendering process :D

        //})

    });
}


//initialize the canvas
function initCanvas(name) {
    canvas = document.getElementById(name);
    canvas.width = window.innerWidth;
    canvas.height = window.innerHeight;
    gl = canvas.getContext("webgl", { antialias: true, depth: true });

    if (!gl) {
        gl = canvas.getContext("experimental-webgl")
    }

    if (!gl) {
        alert("Uw browser ondersteund geen WebGL");
    }

    //set the clear color
    //gl.clearColor(1, 0, 1, 1);

    //clear the canvas
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    //enable depth buffer 
    gl.enable(gl.DEPTH_TEST);

    //disable inside faces for speed improvement
    gl.enable(gl.CULL_FACE);
    gl.frontFace(gl.CCW);
    gl.cullFace(gl.BACK);

    return canvas;
}

//load a to .json converted blender model.
function loadModel(location, callback) {
    //console.log(location);
    initTexture(location, function (modelTexture) {
        //function to be executed when image is loaded:
        //console.log(modelTexture);
        loadText(location + '.ez', function (ezErr, ezText) {
            if (ezErr) {
                alert('Error getting' + location + '.ez');
                console.error(ezErr);
            } else {
                //read model position and shading data:
                var ezData = ezparser.parse(ezText);
                console.log(ezData);

                loadJSON(location + ".json", function (modelErr, modelObj) {
                    if (modelErr) {
                        console.log(modelErr);
                        alert('Error getting' + location + '.json');
                        console.error(modelErr);
                    } else {

                        //console.log(modelObj);
                        //extract vertice and indice information from the model:

                        var modelVertices = [];
                        var modelIndices = [];
                        var modelTexcoords = [];
                        var modelNormals = [];

                        modelVertices = modelObj.meshes[ezData.meshNo].vertices;
                        modelIndices = [].concat.apply([], modelObj.meshes[ezData.meshNo].faces);
                        modelTexcoords = modelObj.meshes[ezData.meshNo].texturecoords[0];
                        modelNormals = modelObj.meshes[ezData.meshNo].normals;


                        //invert the blender model vertices to the axis of webgl
                        modelVertices = blender3DCoordsInverter(modelVertices);

                        //invert the blender normals to the axis of webgl
                        modelNormals = blender3DCoordsInverter(modelNormals);

                        //fill buffers with correct model information
                        var buffers = fillBuffers(modelVertices, modelTexcoords, modelIndices, modelNormals);

                        //create a model object containing all relevant information and return it in the callback
                        var tmpModel = null;

                        if (!ezData.isLightSource) { //if model is a not a lightsource
                            tmpModel = new model(location, ezData.worldCoord, ezData.initialRotation, modelVertices, modelIndices, modelNormals, modelTexcoords, modelTexture, buffers, ezData, true);
                            //add model to modelsArray
                            modelsArray.push(tmpModel);
                        } else {
                            tmpModel = new lightSource(location, ezData.worldCoord, ezData.initialRotation, modelVertices, modelIndices, modelNormals, modelTexcoords, modelTexture, buffers, ezData, true);
                            //add model to lightSourcesArray
                            lightSourcesArray.push(tmpModel);
                        }
                        //return the model object
                        callback(tmpModel);
                    }
                });
            }
        }
        );
    });
}

function initTexture(location, callback) {
    var img = new Image();
    img.onload = function () {
        //console.log(img);
        loadTexture(img, callback);
    }
    img.src = location + ".png";
}

function loadTexture(img, callback) {
    //create texture
    var modelTexture = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D, modelTexture);
    //flip the texture because the coordinates are different in webgl then blender
    gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
    //s,t coordinates
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);

    //specifies the information that needs to be used
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, img);
    gl.bindTexture(gl.TEXTURE_2D, null);

    callback(modelTexture);
}


function loadShaders(callback) {

    loadText(modelviewerpath + 'shaders/shader.vs.glsl', function (vsErr, vsText) {
        if (vsErr) {
            alert('Error getting shader.vs.glsl');
            console.error(vsErr);
        } else {
            loadText(modelviewerpath + 'shaders/shader.fs.glsl', function (fsErr, fsText) {
                if (fsErr) {
                    alert('Error getting shader.fs.glsl');
                    console.error(fsErr);
                } else {
                    callback(vsText, fsText);
                }
            });
        }
    });
};

function compileShaders(vsText, fsText) {
    //model = modelObj;

    //create vertex-shader
    var vertexShader = gl.createShader(gl.VERTEX_SHADER);
    gl.shaderSource(vertexShader, vsText);

    //create fragment-shader
    var fragmentShader = gl.createShader(gl.FRAGMENT_SHADER);
    gl.shaderSource(fragmentShader, fsText);

    gl.compileShader(vertexShader);
    //log any compile error's
    if (!gl.getShaderParameter(vertexShader, gl.COMPILE_STATUS)) {
        console.error('vertexShader compile error: ', gl.getShaderInfoLog(vertexShader));
    }
    gl.compileShader(fragmentShader);
    //log any compile error's
    if (!gl.getShaderParameter(fragmentShader, gl.COMPILE_STATUS)) {
        console.error('fragmentShader compile error: ', gl.getShaderInfoLog(fragmentShader));
    }
    return createProgram(vertexShader, fragmentShader);
}


function createProgram(vertexShader, fragmentShader) {
    //create program
    var program = gl.createProgram();
    gl.attachShader(program, vertexShader);
    gl.attachShader(program, fragmentShader);
    gl.linkProgram(program);
    if (!gl.getProgramParameter(program, gl.LINK_STATUS)) {
        console.error('ERROR linking program', gl.getProgramInfoLog(program));
        return null;
    }
    gl.validateProgram(program);
    if (!gl.getProgramParameter(program, gl.VALIDATE_STATUS)) {
        console.error('ERROR validating program', gl.getProgramInfoLog(program));
        return null;
    }

    return program;
}

//Destroys the buffers
function killBuffers(buffers) {
    if (buffers != null) {
        gl.deleteBuffer(buffers.vertexBuffer);
        gl.deleteBuffer(buffers.texcoordsBuffer);
        gl.deleteBuffer(buffers.normalsBuffer);
    }
}

function fillBuffers(modelVertices, modelTexcoords, modelIndices, modelNormals) {
    //create vertexBuffer
    var vertexBuffer = gl.createBuffer()
    gl.bindBuffer(gl.ARRAY_BUFFER, vertexBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(modelVertices), gl.STATIC_DRAW);

    //console.log(modelVertices);

    //create texcoordsBuffer
    var texcoordsBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, texcoordsBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(modelTexcoords), gl.STATIC_DRAW);

    //create indexBuffer
    var indexBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, indexBuffer);
    gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(modelIndices), gl.STATIC_DRAW);

    //create normalsBuffer
    var normalsBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, normalsBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(modelNormals), gl.STATIC_DRAW);

    return { vertexBuffer: vertexBuffer, texcoordsBuffer: texcoordsBuffer, indexBuffer: indexBuffer, normalsBuffer: normalsBuffer };
};


function bindBuffers(buffers, program, worldMatrix, ezData) {
    gl.bindBuffer(gl.ARRAY_BUFFER, buffers.vertexBuffer);
    var positionAttrib = gl.getAttribLocation(program, 'position');
    //attributelocation,n0 of elements in attribute (e.g. vec2),type,is normalized,size of individual vertex,offset from beginning of a single vertex to this attribute (om bij de kleuren te komen in array eerste 2 skippen bvb.)
    gl.vertexAttribPointer(positionAttrib, 3, gl.FLOAT, false, 3 * Float32Array.BYTES_PER_ELEMENT, 0);
    gl.enableVertexAttribArray(positionAttrib);

    gl.bindBuffer(gl.ARRAY_BUFFER, buffers.texcoordsBuffer);
    var textureCoordAttrib = gl.getAttribLocation(program, 'textureCoord');
    gl.vertexAttribPointer(textureCoordAttrib, 2, gl.FLOAT, false, 2 * Float32Array.BYTES_PER_ELEMENT, 0);
    gl.enableVertexAttribArray(textureCoordAttrib);

    gl.bindBuffer(gl.ARRAY_BUFFER, buffers.normalsBuffer);
    var normalAttrib = gl.getAttribLocation(program, 'normal');
    gl.vertexAttribPointer(normalAttrib, 3, gl.FLOAT, true, 3 * Float32Array.BYTES_PER_ELEMENT, 0);
    gl.enableVertexAttribArray(normalAttrib);

    //use program
    gl.useProgram(program);

    var worldMatrixUniform = gl.getUniformLocation(program, 'worldMatrix');
    var viewMatrixUniform = gl.getUniformLocation(program, 'viewMatrix');
    var projMatrixUniform = gl.getUniformLocation(program, 'projMatrix');
    var ambientReflCoefUniform = gl.getUniformLocation(program, 'ambientReflCoef');
    var diffuseReflCoefUniform = gl.getUniformLocation(program, 'diffuseReflCoef');
    var specularReflCoefUniform = gl.getUniformLocation(program, 'specularReflCoef');
    var shininessCoefUniform = gl.getUniformLocation(program, 'shininessCoef');
    var viewerPositionUniform = gl.getUniformLocation(program, 'viewerPosition');
    var lightPositionUniform = gl.getUniformLocation(program, 'lightPosition');
    var lightIntensityUniform = gl.getUniformLocation(program, 'lightIntensity');

    var lightPosition = new Float32Array(3);
    lightPosition[0] = lightSourcesArray[0].worldMatrix[12];
    lightPosition[1] = lightSourcesArray[0].worldMatrix[13];
    lightPosition[2] = lightSourcesArray[0].worldMatrix[14];

    //console.log(worldMatrix);

    //no support for multiple lightsources yet
    var lightIntensity = new Float32Array(3);
    lightIntensity[0] = lightSourcesArray[0].ezData.lightIntensity[0];
    lightIntensity[1] = lightSourcesArray[0].ezData.lightIntensity[1];
    lightIntensity[2] = lightSourcesArray[0].ezData.lightIntensity[2];


    gl.uniform3fv(lightPositionUniform, lightPosition);
    gl.uniform3fv(lightIntensityUniform, lightIntensity);

    //apply all matrices to the uniform
    gl.uniformMatrix4fv(worldMatrixUniform, gl.FALSE, worldMatrix);
    gl.uniformMatrix4fv(viewMatrixUniform, gl.FALSE, cameraViewer.viewMatrix);
    gl.uniformMatrix4fv(projMatrixUniform, gl.FALSE, cameraViewer.projMatrix);

    var viewerPosition = new Float32Array(3);
    viewerPosition[0] = cameraViewer.viewMatrix[12];
    viewerPosition[1] = cameraViewer.viewMatrix[13];
    viewerPosition[2] = cameraViewer.viewMatrix[14];

    //console.log(viewMatrix);

    gl.uniform3fv(viewerPositionUniform, viewerPosition)

    //only if ezData is specified, use its values
    if (ezData != null) {
        gl.uniform1f(ambientReflCoefUniform, ezData.ambientReflCoef);
        gl.uniform1f(diffuseReflCoefUniform, ezData.diffuseReflCoef);
    } else {
        gl.uniform1f(ambientReflCoefUniform, 1.0);
        gl.uniform1f(diffuseReflCoefUniform, 0.0);
    }
}



//Change the coordinates of the blender model so it matches the webGL coordinates
function blender3DCoordsInverter(vertices) {
    var returnVertices = new Array(vertices.length);
    for (var i = 0; i < vertices.length; i = i + 3) {
        returnVertices[i] = -vertices[i];
        returnVertices[i + 1] = vertices[i + 2];
        returnVertices[i + 2] = vertices[i + 1];
    }

    return returnVertices;

}

//Update a textfield with new text
function updateTextField(userText, textObj) {
    //Generate a new texture:
    generateTextFieldTexture(userText, textObj.textFieldCreationData, function (texture) {
        textObj.texture = texture; //update the texture
        textObj.userText = userText; //update the name
    });
}

function generateSingleColorTexture(color, callback) {
    //configure the 2d canvas for displaying a color
    textContext = document.getElementById("texture_create_canvas").getContext("2d");
    textContext.canvas.width = 32;
    textContext.canvas.height = 32;
    textContext.fillStyle = color;
    //fill the canvas with the color
    textContext.fillRect(0, 0, textContext.canvas.width, textContext.canvas.height);

    //convert canvas to image
    var image = new Image();

    //load the image
    image.onload = function () {
        //Gets executed when image is loaded:
        loadTexture(image, function (texture) {
            //Gets executed when texture is loaded:
            callback(texture);
        });
    }

    //source of the image to be loaded
    image.src = textContext.canvas.toDataURL("image/png");
}

function createGrass(color, callback) {
    generateSingleColorTexture(color, function (texture) {
        //vertices data for a triangle / grasspiece
        var modelVertices = [
            -0.1, -1.0, 0.0,
            0.0, 1.0, 0.0,
            0.1, -1.0, 0.0
        ];

        var modelTexcoords = [
            0, 0,
            1, 0,
            0.5, 1
        ];

        var modelIndices = [ ///only 1 triangle so no indices
        ];

        var modelNormals = [
            0, 0, 1,
            0, 0, 1,
            0, 0, 1,
        ];

        var buffers = fillBuffers(modelVertices, modelTexcoords, modelIndices, modelNormals);
        //console.log(buffers);
        grassField = new grass(color, buffers, texture, false); //fill the grassfield with data
        callback(); //callback indicating its done
    })
}



//genereta a webgl texture from the userinput using a 2d canvas
function generateTextFieldTexture(userText, textFieldCreationData, callback) {
    //configure the 2d canvas for displaying the text
    textContext = document.getElementById("texture_create_canvas").getContext("2d");
    textContext.canvas.width = 1024;
    textContext.canvas.height = 1024;

    // flip context horizontally
    textContext.scale(1, 1);

    //text properties
    textContext.font = textFieldCreationData.size.toString() + "px " + textFieldCreationData.font;
    textContext.textAlign = textFieldCreationData.allign;
    textContext.textBaseline = "middle";

    //clear the canvas
    textContext.clearRect(0, 0, textContext.canvas.width, textContext.canvas.height);

    //fill the background
    textContext.fillStyle = textFieldCreationData.bgColor;
    textContext.fillRect(0, 0, textContext.canvas.width, textContext.canvas.height);

    //write the text;
    textContext.fillStyle = textFieldCreationData.textColor;
    textContext.fillText(userText, textContext.canvas.width / 2, textContext.canvas.height / 2);

    //convert canvas to image
    var image = new Image();

    //load the image
    image.onload = function () {
        //Gets executed when image is loaded:
        loadTexture(image, function (texture) {
            //Gets executed when texture is loaded:
            callback(texture);
        });
    }

    //source of the image to be loaded
    image.src = textContext.canvas.toDataURL("image/png");
}

function createTextField(name, userText, textColor, bgColor, size, allign, font, x, y, z, theta, callback) {
    //put all values into an object
    var textFieldCreationData = { name: name, textColor: textColor, bgColor: bgColor, size: size, allign: allign, font: font, x: x, y: y, z: z, theta: theta };

    //genereta a webgl texture from the userinput using a 2d canvas
    generateTextFieldTexture(userText, textFieldCreationData, function (texture) {

        //vertices data for a round / textfield
        var modelVertices = [0.996194, 0.0871559, 2.38419e-7,
            0.994056, -0.108867, 2.38419e-7,
            0.953717, -0.300706, 2.38419e-7,
            0.876727, -0.480989, 2.38419e-7,
            0.766044, -0.642787, 2.38419e-7,
            0.625923, -0.779884, 2.98023e-7,
            0.461748, -0.887011, 2.68221e-7,
            0.279829, -0.96005, 2.83122e-7,
            0.0871556, -0.996195, 2.98023e-7,
            -0.108867, -0.994056, 3.12924e-7,
            -0.300706, -0.953717, 3.27826e-7,
            -0.480989, -0.876727, 2.98023e-7,
            -0.642788, -0.766044, 3.57628e-7,
            -0.779885, -0.625923, 3.57628e-7,
            -0.887011, -0.461748, 3.57628e-7,
            -0.96005, -0.279829, 3.57628e-7,
            -0.996195, -0.0871554, 3.57628e-7,
            -0.994057, 0.108867, 3.57628e-7,
            -0.953717, 0.300706, 3.57628e-7,
            -0.876727, 0.480989, 3.57628e-7,
            -0.766044, 0.642788, 3.57628e-7,
            -0.625923, 0.779885, 2.98023e-7,
            -0.461748, 0.887011, 3.27826e-7,
            -0.279828, 0.96005, 3.12924e-7,
            -0.087155, 0.996195, 2.98023e-7,
            0.108868, 0.994056, 2.83122e-7,
            0.300707, 0.953717, 2.68221e-7,
            0.48099, 0.876726, 2.98023e-7,
            0.642788, 0.766044, 2.38419e-7,
            0.779885, 0.625922, 2.38419e-7,
            0.887011, 0.461747, 2.38419e-7,
            0.96005, 0.279828, 2.38419e-7,
            0.96005, 0.279828, 2.38419e-7,
        ];

        var modelIndices = [31, 0, 1,
            31, 1, 2,
            31, 2, 3,
            31, 3, 4,
            31, 4, 5,
            31, 5, 6,
            31, 6, 7,
            31, 7, 8,
            31, 8, 9,
            31, 9, 10,
            31, 10, 11,
            31, 11, 12,
            31, 12, 13,
            31, 13, 14,
            31, 14, 15,
            31, 15, 16,
            31, 16, 17,
            31, 17, 18,
            31, 18, 19,
            31, 19, 20,
            31, 20, 21,
            31, 21, 22,
            31, 22, 23,
            31, 23, 24,
            31, 24, 25,
            31, 25, 26,
            31, 26, 27,
            31, 27, 28,
            31, 28, 29,
            29, 30, 31,
            29, 30, 31,
        ];

        var modelNormals = [-6.20996e-8, -5.43299e-9, -1,
            -6.20996e-8, -5.43299e-9, -1,
            -6.20996e-8, -5.43299e-9, -1,
            -6.20996e-8, -5.43299e-9, -1,
            -6.20996e-8, -5.43299e-9, -1,
            -6.20996e-8, -5.43299e-9, -1,
            -6.20996e-8, -5.43299e-9, -1,
            -6.20996e-8, -5.43299e-9, -1,
            -6.20996e-8, -5.43299e-9, -1,
            -6.20996e-8, -5.43299e-9, -1,
            -6.20996e-8, -5.43299e-9, -1,
            -6.20996e-8, -5.43299e-9, -1,
            -6.20996e-8, -5.43299e-9, -1,
            -6.20996e-8, -5.43299e-9, -1,
            -6.20996e-8, -5.43299e-9, -1,
            -6.20996e-8, -5.43299e-9, -1,
            -6.20996e-8, -5.43299e-9, -1,
            -6.20996e-8, -5.43299e-9, -1,
            -6.20996e-8, -5.43299e-9, -1,
            -6.20996e-8, -5.43299e-9, -1,
            -6.20996e-8, -5.43299e-9, -1,
            -6.20996e-8, -5.43299e-9, -1,
            -6.20996e-8, -5.43299e-9, -1,
            -6.20996e-8, -5.43299e-9, -1,
            -6.20996e-8, -5.43299e-9, -1,
            -6.20996e-8, -5.43299e-9, -1,
            -6.20996e-8, -5.43299e-9, -1,
            -6.20996e-8, -5.43299e-9, -1,
            -6.20996e-8, -5.43299e-9, -1,
            -6.20996e-8, -5.43299e-9, -1,
            -6.20996e-8, -5.43299e-9, -1,
            -6.20996e-8, -5.43299e-9, -1,
            -6.20996e-8, -5.43299e-9, -1,
        ];

        var modelTexcoords = [0, 0.549246,
            0, 0.450755,
            0.0192146, 0.354156,
            0.0569056, 0.263161,
            0.111624, 0.181269,
            0.181268, 0.111625,
            0.263161, 0.0569058,
            0.354155, 0.0192148,
            0.450754, 8.98396e-8,
            0.549245, 0,
            0.645844, 0.0192147,
            0.736839, 0.0569057,
            0.818731, 0.111625,
            0.888375, 0.181269,
            0.943094, 0.263161,
            0.980785, 0.354155,
            1, 0.450754,
            1, 0.549246,
            0.980785, 0.645845,
            0.943094, 0.736839,
            0.888375, 0.818732,
            0.818731, 0.888376,
            0.736839, 0.943094,
            0.645844, 0.980785,
            0.549245, 1,
            0.450754, 1,
            0.354155, 0.980785,
            0.263161, 0.943094,
            0.181268, 0.888375,
            0.111624, 0.818731,
            0.0569055, 0.736838,
            0.0192145, 0.645844,
            0.0192145, 0.645844,
        ];

        //variable to hold the position values;
        var position = new vec3.fromValues(x, y, z);

        //variable to hold the rotation values;
        var rotation = new vec3.fromValues(0, 0, 0);

        //fill the buffers
        var buffers = fillBuffers(modelVertices, modelTexcoords, modelIndices, modelNormals);

        var tmpText = null;
        //make a new text model:
        tmpText = new text(name, position, rotation, modelVertices, modelIndices, modelNormals, modelTexcoords, texture, buffers, true, userText, textFieldCreationData);

        //add the text model to the textArray
        textArray.push(tmpText);

        //callback text object when done;
        callback(tmpText);
    })
}

//when in debug mode the camera position can be moved with the arrow + wasd keys
function moveCamera(deltaTime) {
    switch (keyCode) {
        //left arrow
        case 37:
            cameraViewer.translate(deltaTime * cameraMovementSpeed, 0, 0); //left
            break;
        //up arrow
        case 38:
            cameraViewer.translate(0, 0, deltaTime * cameraMovementSpeed); //forwards
            break;
        //right arrow
        case 39:
            cameraViewer.translate(-deltaTime * cameraMovementSpeed, 0, 0); //right
            break;
        //down arrow
        case 40:
            cameraViewer.translate(0, 0, -deltaTime * cameraMovementSpeed); //backwards
            break;
        //w key
        case 87:
            cameraViewer.translate(0, -deltaTime * cameraMovementSpeed, 0); //up
            break;
        //s key
        case 83:
            cameraViewer.translate(0, deltaTime * cameraMovementSpeed, 0); //down
            break;
        //a key
        case 65:
            cameraViewer.rotateLocal(0, -deltaTime * cameraRotateSpeed, 0); //rotate left 
            break;
        //d key
        case 68:
            cameraViewer.rotateLocal(0, deltaTime * cameraRotateSpeed, 0); //rotate right
            break;
        default:
            break;
    }
}

//</FUNCTIONS>//

//<OBJECT CONSTRUCTORS>//

//constructor for camera object
function camera() {
    this.viewMatrix = mat4.create();
    this.projMatrix = mat4.create();
}

//specifies the position of the camera
camera.prototype.position = function (x, y, z) {
    //translate the identitymatrix and set in worldmatrix
    mat4.translate(this.viewMatrix, identityMatrix, vec3.fromValues(-x, y, -z));
}

//translate the camera
camera.prototype.translate = function (x, y, z) {
    mat4.translate(this.viewMatrix, this.viewMatrix, vec3.fromValues(-x, y, -z));
}

//rotate the camera around its own axis
camera.prototype.rotateLocal = function (x_angle, y_angle, z_angle) {
    //rotate the camera
    //rotate x 
    mat4.rotate(this.viewMatrix, this.viewMatrix, (x_angle / 360) * 2 * Math.PI, [1.0, 0.0, 0.0]);
    //rotate y
    mat4.rotate(this.viewMatrix, this.viewMatrix, (y_angle / 360) * 2 * Math.PI, [0.0, 1.0, 0.0]);
    //rotate z
    mat4.rotate(this.viewMatrix, this.viewMatrix, (z_angle / 360) * 2 * Math.PI, [0.0, 0.0, 1.0]);
}

//constructor for grass object
function grass(color, buffers, texture, active) {
    this.color = color;
    this.texture = texture;
    this.buffers = buffers;
    this.active = active;
    this.worldMatrix = mat4.create();
}

//enable the grass
model.prototype.enable = function () {
    this.active = true;
};

//enable the grass
model.prototype.disable = function () {
    this.active = false;
};

//constructor for Model object
function model(name, position, rotation, vertices, indices, normals, texCoords, texture, buffers, ezData, active) {
    this.name = name;
    this.position = position; //vec3
    this.rotation = rotation; //vec3
    this.scale = vec3.fromValues(1, 1, 1); //default value 
    this.vertices = vertices;
    this.indices = indices;
    this.normals = normals;
    this.texCoords = texCoords;
    this.texture = texture;
    this.buffers = buffers;
    this.ezData = ezData;
    this.worldMatrix = mat4.create(); //create a worldmatrix and apply the initial position and rotation.
    if (this.positon != null) //only execute if position was specified while constructing the object
    {
        mat4.translate(this.worldMatrix, this.worldMatrix, this.position); //translate the worldmatrix with the position value
        mat4.rotate(this.worldMatrix, this.worldMatrix, (rotation[0] / 360) * 2 * Math.PI, [1.0, 0.0, 0.0]); //rotate around the x-axis
        mat4.rotate(this.worldMatrix, this.worldMatrix, (rotation[1] / 360) * 2 * Math.PI, [0.0, 1.0, 0.0]); //rotate around the y-axis
        mat4.rotate(this.worldMatrix, this.worldMatrix, (rotation[2] / 360) * 2 * Math.PI, [0.0, 0.0, 1.0]); //rotate around the z-axis
    }
    this.active = active;
    this.childs = []; //array of childs
    this.parent = null;
}

//Prototype functions for model object:

model.prototype.isActive = function () { return this.active }; //check if active

//enable the model
model.prototype.enable = function () {
    this.active = true;
};

//enable the model
model.prototype.disable = function () {
    this.active = false;
};

//add a child to the model
model.prototype.addChild = function (childModel) {
    if (childModel != null) {
        this.childs.push(childModel); //add child to "this" model
        childModel.parent = this;     //add parent to the child
    } else {
        console.log("No (correct) child object specified!");
    }
}; //add a childs to the model and add parent to the child

//Set the position of the model (and childs)
model.prototype.positionGlobal = function (x, y, z) {
    vec3.set(this.position, -x, y, -z); //set the new position
    mat4.translate(this.worldMatrix, this.worldMatrix, this.position); //apply the position to the wordlmatrix, keeping its rotation

    //if the model has any childs, translate them too
    for (var i = 0; i < this.childs.length; i++) {
        vec3.set(this.childs[i].position, -x, y, -z); //set the new position for the child
        mat4.translate(this.childs[i].worldMatrix, this.childs[i].worldMatrix, this.childs[i].position); //apply the position to its wordlmatrix, keeping its rotation
    }
}

//Translate the model (and childs)
model.prototype.translate = function (x, y, z) {
    vec3.add(this.position, this.position, vec3.fromValues(-x, y, -z)); //translate the position by adding the values to the vec3 position
    mat4.translate(this.worldMatrix, this.worldMatrix, vec3.fromValues(-x, y, -z)); //apply the position to the wordlmatrix, keeping its rotation


    //if the model has any childs, translate them too
    for (var i = 0; i < this.childs.length; i++) {
        vec3.add(this.childs[i].position, this.childs[i].position, vec3.fromValues(-x, y, -z)); //translate the position of child by adding the values to the vec3 position
        mat4.translate(this.childs[i].worldMatrix, this.childs[i].worldMatrix, vec3.fromValues(-x, y, -z)); //apply the position to its wordlmatrix, keeping its rotation
    }
}

//scale the model with a certain factor
model.prototype.scaleLocal = function (x_factor, y_factor, z_factor) {
    //Scale the wordlmatrix
    vec3.set(this.scale, x_factor, y_factor, z_factor);
    mat4.scale(this.worldMatrix, this.worldMatrix, this.scale);

    //if the model has any childs, scale them too
    for (var i = 0; i < this.childs.length; i++) {
        vec3.set(this.childs[i].scale, x_factor, y_factor, z_factor);
        mat4.scale(this.childs[i].worldMatrix, this.childs[i].worldMatrix, this.childs[i].scale);
    }
}

//Specify the angle of the model (and childs)
model.prototype.angleLocal = function (x_angle, y_angle, z_angle) {
    //calculate the delta angle to turn (rotate it back to 0 and add the x,y,z angles)
    var delta_x = x_angle - this.rotation[0];
    var delta_y = y_angle - this.rotation[1];
    var delta_z = z_angle - this.rotation[2];

    vec3.set(this.rotation, x_angle, y_angle, z_angle);

    //rotate the model
    //rotate x 
    mat4.rotate(this.worldMatrix, this.worldMatrix, (delta_x / 360) * 2 * Math.PI, [1.0, 0.0, 0.0]);
    //rotate y
    mat4.rotate(this.worldMatrix, this.worldMatrix, (delta_y / 360) * 2 * Math.PI, [0.0, 1.0, 0.0]);
    //rotate z
    mat4.rotate(this.worldMatrix, this.worldMatrix, (delta_z / 360) * 2 * Math.PI, [0.0, 0.0, 1.0]);

    //if the model has any childs, rotate them too
    for (var i = 0; i < this.childs.length; i++) {
        //calculate the delta angle to turn 
        delta_x = x_angle - this.childs[i].rotation[0];
        delta_y = y_angle - this.childs[i].rotation[1];
        delta_z = z_angle - this.childs[i].rotation[2];

        vec3.set(this.childs[i].rotation, vec3, x_angle, y_angle, z_angle);

        //rotate x 
        mat4.rotate(this.childs[i].worldMatrix, this.childs[i].worldMatrix, (delta_x / 360) * 2 * Math.PI, [1.0, 0.0, 0.0]);
        //rotate y
        mat4.rotate(this.childs[i].worldMatrix, this.childs[i].worldMatrix, (delta_y / 360) * 2 * Math.PI, [0.0, 1.0, 0.0]);
        //rotate z
        mat4.rotate(this.childs[i].worldMatrix, this.childs[i].worldMatrix, (delta_z / 360) * 2 * Math.PI, [0.0, 0.0, 1.0]);

    }
}

//rotate the model (and childs) about its own axis with a certain degree
model.prototype.rotateLocal = function (x_angle, y_angle, z_angle) {
    vec3.add(this.rotation, this.rotation, vec3.fromValues(x_angle, y_angle, z_angle));

    //rotate the model
    //rotate x 
    mat4.rotate(this.worldMatrix, this.worldMatrix, (x_angle / 360) * 2 * Math.PI, [1.0, 0.0, 0.0]);
    //rotate y
    mat4.rotate(this.worldMatrix, this.worldMatrix, (y_angle / 360) * 2 * Math.PI, [0.0, 1.0, 0.0]);
    //rotate z
    mat4.rotate(this.worldMatrix, this.worldMatrix, (z_angle / 360) * 2 * Math.PI, [0.0, 0.0, 1.0]);

    //if the model has any childs, rotate them too
    for (var i = 0; i < this.childs.length; i++) {
        //rotate the worldmatrix of the parent

        //rotate x 
        mat4.rotate(this.childs[i].worldMatrix, this.worldMatrix, (x_angle + this.childs[i].rotation[0] / 360) * 2 * Math.PI, [1.0, 0.0, 0.0]);
        //rotate y
        mat4.rotate(this.childs[i].worldMatrix, this.worldMatrix, (y_angle + this.childs[i].rotation[1] / 360) * 2 * Math.PI, [0.0, 1.0, 0.0]);
        //rotate z
        mat4.rotate(this.childs[i].worldMatrix, this.worldMatrix, (z_angle + this.childs[i].rotation[2] / 360) * 2 * Math.PI, [0.0, 0.0, 1.0]);

        //translate, scale the child back after the rotation:
        mat4.translate(this.childs[i].worldMatrix, this.childs[i].worldMatrix, this.childs[i].position);
        mat4.scale(this.childs[i].worldMatrix, this.childs[i].worldMatrix, this.childs[i].scale);

    }
}

//rotate the model (and childs) about the global axis
model.prototype.rotateGlobal = function (x_angle, y_angle, z_angle) {
    //rotate the model
    this.rotation = vec3.fromValues(x_angle, y_angle, z_angle);
    //rotate x 
    mat4.rotate(this.worldMatrix, identityMatrix, (x_angle / 360) * 2 * Math.PI, [1.0, 0.0, 0.0]);
    //rotate y
    mat4.rotate(this.worldMatrix, identityMatrix, (y_angle / 360) * 2 * Math.PI, [0.0, 1.0, 0.0]);
    //rotate z
    mat4.rotate(this.worldMatrix, identityMatrix, (z_angle / 360) * 2 * Math.PI, [0.0, 0.0, 1.0]);

    //translate and scale it back after rotation:
    mat4.translate(this.worldMatrix, this.worldMatrix, this.position);
    mat4.scale(this.worldMatrix, this.worldMatrix, this.scale);

    //if the model has any childs, rotate them too
    for (var i = 0; i < this.childs.length; i++) {
        //rotate x 
        mat4.rotate(this.childs[i].worldMatrix, identityMatrix, (x_angle / 360) * 2 * Math.PI, [1.0, 0.0, 0.0]);
        //rotate y
        mat4.rotate(this.childs[i].worldMatrix, identityMatrix, (y_angle / 360) * 2 * Math.PI, [0.0, 1.0, 0.0]);
        //rotate z
        mat4.rotate(this.childs[i].worldMatrix, identityMatrix, (z_angle / 360) * 2 * Math.PI, [0.0, 0.0, 1.0]);

        //translate and scale it back after rotation:
        mat4.translate(this.childs[i].worldMatrix, this.childs[i].worldMatrix, this.childs[i].position);
        mat4.scale(this.childs[i].worldMatrix, this.childs[i].worldMatrix, this.childs[i].scale);
    }

}

//constructor for lightsource object
function lightSource(name, position, rotation, vertices, indices, normals, texCoords, texture, buffers, ezData, active) {
    this.name = name;
    this.position = position; //vec3
    this.rotation = rotation; //vec3
    this.scale = vec3.fromValues(1, 1, 1); //default value 
    this.vertices = vertices;
    this.indices = indices;
    this.normals = normals;
    this.texCoords = texCoords;
    this.texture = texture;
    this.buffers = buffers;
    this.ezData = ezData;
    this.worldMatrix = mat4.create(); //create a worldmatrix and apply the initial position and rotation.
    if (this.positon != null) //only execute if position was specified while constructing the object
    {
        mat4.translate(this.worldMatrix, this.worldMatrix, this.position); //translate the worldmatrix with the position value
        mat4.rotate(this.worldMatrix, this.worldMatrix, (rotation[0] / 360) * 2 * Math.PI, [1.0, 0.0, 0.0]); //rotate around the x-axis
        mat4.rotate(this.worldMatrix, this.worldMatrix, (rotation[1] / 360) * 2 * Math.PI, [0.0, 1.0, 0.0]); //rotate around the y-axis
        mat4.rotate(this.worldMatrix, this.worldMatrix, (rotation[2] / 360) * 2 * Math.PI, [0.0, 0.0, 1.0]); //rotate around the z-axis
    }
    this.active = active;
    this.childs = []; //array of childs
    this.parent = null;
}

//lightSource is a subclass of a model
lightSource.prototype = new model();

//redirect the constructor
lightSource.prototype.constructor = lightSource;

//constructor for Text(field) object
function text(name, position, rotation, vertices, indices, normals, texCoords, texture, buffers, active, text, textFieldCreationData) {
    this.name = name;
    this.position = position; //vec3
    this.rotation = rotation; //vec3
    this.scale = vec3.fromValues(1, 1, 1); //default value 
    this.vertices = vertices;
    this.indices = indices;
    this.normals = normals;
    this.texCoords = texCoords;
    this.texture = texture;
    this.buffers = buffers;
    this.worldMatrix = mat4.create(); //create a worldmatrix and apply the initial position and rotation.
    if (this.positon != null) //only execute if position was specified while constructing the object
    {
        mat4.translate(this.worldMatrix, this.worldMatrix, this.position); //translate the worldmatrix with the position value
        mat4.rotate(this.worldMatrix, this.worldMatrix, (rotation[0] / 360) * 2 * Math.PI, [1.0, 0.0, 0.0]); //rotate around the x-axis
        mat4.rotate(this.worldMatrix, this.worldMatrix, (rotation[1] / 360) * 2 * Math.PI, [0.0, 1.0, 0.0]); //rotate around the y-axis
        mat4.rotate(this.worldMatrix, this.worldMatrix, (rotation[2] / 360) * 2 * Math.PI, [0.0, 0.0, 1.0]); //rotate around the z-axis
    }
    this.active = active;
    this.childs = []; //array of childs
    this.parent = null;
    this.text = text;
    this.textFieldCreationData = textFieldCreationData;
}

//text is a subclass of a model
text.prototype = new model();

//redirect the constructor
text.prototype.constructor = text;

//adjust the text on the textfield
text.prototype.updateTextField = function (txt) {
    updateTextField(txt, this); //update the textfield
}

var tempMat4 = mat4.create();
//</OBJECT CONSTRUCTORS>//

var grassTimer = 0;

//<MAIN RENDER LOOP>//
var renderLoop = function (now) {
    gl.useProgram(program);

    //clear color and depth buffer
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    //Deltatime is calculated so the model tuning speed is independent of the framerate / performance of the computer

    // Convert the time to seconds
    now *= 0.001;
    // Subtract the previous time from the current time
    var deltaTime = now - then;
    // Remember the current time for the next frame.
    then = now;

    //call the fixedUpdate function in user specified script.
    fixedUpdate(deltaTime);

    //if debug mode is on and the user is holding a key
    if (debugMode && holding) {
        moveCamera(deltaTime); //move the camera according to the user's input
    }


    if (grassField.active) { //if grass is activated
        if (grassTimer <= 20) {
            mat4.translate(grassField.worldMatrix, grassField.worldMatrix, vec3.fromValues(0.01, 0, 0));
        } else if (grassTimer <= 40) {
            mat4.translate(grassField.worldMatrix, grassField.worldMatrix, vec3.fromValues(-0.01, 0, 0));
        } else {
            grassTimer = 0;
        }

        grassTimer++;

        //fill plane with grass
        for (var i = 0; i < grassPlaneWidth; i++) {
            for (var j = 0; j < grassPlaneHeight; j++) {
                tempMat4 = mat4.clone(identityMatrix);
                mat4.translate(tempMat4, grassField.worldMatrix, vec3.fromValues(i / grassDensity, 0, j / grassDensity));

                mat4.rotate(tempMat4, tempMat4, Math.random() / 10, [1.0, 0.0, 0.0]);
                mat4.rotate(tempMat4, tempMat4, Math.random() / 10, [0.0, 0.0, 1.0]);

                //bind the correct buffers
                bindBuffers(grassField.buffers, program, tempMat4, null);

                //bind textures of model no. i
                gl.bindTexture(gl.TEXTURE_2D, grassField.texture);
                gl.activeTexture(gl.TEXTURE0);

                //bind indexbuffer
                //gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, grassField.buffers.indexBuffer);

                //draw vertices of model no. i
                gl.drawArrays(gl.TRIANGLES, 0, 3);

            }
        }
    }


    //loop over all textfields
    for (var i = 0; i < textArray.length; i++) {
        //only draw model when activated
        if (textArray[i].active) {
            //console.log(worldMatrixArray[i]);

            //rotate the text
            //textArray[i].theta = deltaTime * 2.0 * Math.PI;
            //mat4.rotate(textArray[i].worldMatrix, textArray[i].worldMatrix, textArray[i].theta, [0.0, 1.0, 0.0]); //auto-rotate around the y-axis 


            //bind the correct buffers
            //console.log(modelsArray[i]);
            bindBuffers(textArray[i].buffers, program, textArray[i].worldMatrix, null);

            //bind textures of model no. i
            gl.bindTexture(gl.TEXTURE_2D, textArray[i].texture);
            gl.activeTexture(gl.TEXTURE0);

            //bind indexbuffer
            gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, textArray[i].buffers.indexBuffer);

            //draw vertices of model no. i
            gl.drawElements(gl.TRIANGLES, textArray[i].indices.length, gl.UNSIGNED_SHORT, 0);
        }
    }

    //loop over all models
    for (var i = 0; i < modelsArray.length; i++) {
        //only draw model when activated
        if (modelsArray[i].active) {
            //console.log(worldMatrixArray[i]);

            //if the user is not rotating the station or userrotation is turned off for the specific model, auto-rotate it if auto-rotation is enabled
            if ((!dragging | !modelsArray[i].ezData.userRotation) & modelsArray[i].ezData.autoRotate) {
                modelsArray[i].rotateLocal(0, deltaTime * modelsArray[i].ezData.rotationSpeed, 0); //auto rotate the model and children if it has any

            }

            //bind the correct buffers
            bindBuffers(modelsArray[i].buffers, program, modelsArray[i].worldMatrix, modelsArray[i].ezData);

            //bind textures of model no. i
            gl.bindTexture(gl.TEXTURE_2D, modelsArray[i].texture);
            gl.activeTexture(gl.TEXTURE0);

            //bind indexbuffer
            gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, modelsArray[i].buffers.indexBuffer);

            //draw vertices of model no. i
            gl.drawElements(gl.TRIANGLES, modelsArray[i].indices.length, gl.UNSIGNED_SHORT, 0);
        }
    }

    //loop over all lightsources
    for (var i = 0; i < lightSourcesArray.length; i++) {
        //only draw lightsource when activated

        if (lightSourcesArray[i].active) {
            //console.log(worldMatrixArray[i]);

            //if the user is not rotating the station or userrotation is turned off for the specific model, auto-rotate it if auto-rotation is enabled
            if ((!dragging | !lightSourcesArray[i].ezData.userRotation) & lightSourcesArray[i].ezData.autoRotate) {
                modelsArray[i].rotateLocal(0, deltaTime * lightSourcesArray[i].ezData.rotationSpeed, 0); //auto rotate the lightsource and children if it has any
            }

            //bind the correct buffers
            //console.log(modelsArray[i]);
            bindBuffers(lightSourcesArray[i].buffers, program, lightSourcesArray[i].worldMatrix, lightSourcesArray[i].ezData);

            //bind textures of model no. i
            gl.bindTexture(gl.TEXTURE_2D, lightSourcesArray[i].texture);
            gl.activeTexture(gl.TEXTURE0);

            //bind indexbuffer
            gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, lightSourcesArray[i].buffers.indexBuffer);

            //draw vertices of model no. i
            gl.drawElements(gl.TRIANGLES, lightSourcesArray[i].indices.length, gl.UNSIGNED_SHORT, 0);
        }
    }

    //loop
    if (!pause) {
        requestAnimationFrame(renderLoop);
    }
};
//</MAIN RENDER LOOP>//
