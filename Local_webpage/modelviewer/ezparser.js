//Parser for the "Eigen-Zand" file format

var ezparser = {};

ezparser.parse = function (data) {

    //default values:
    var worldCoord = vec3.fromValues(0,0,0);
    var ambientReflCoef = 0;
    var diffuseReflCoef = 0;
    var specularReflCoef = 0;
    var initialRotation = vec3.fromValues(0,0,0);
    var shininessCoef = 0;
    var autoRotate = false;
    var rotationSpeed = 0;
    var meshNo = 0;
    var userRotation = false;
    var isLightSource = false;
    var lightIntensity = vec3.fromValues(0,0,0);

    data = data.split("\n");

    for (var i = 0; i < data.length; i++) {
        var temp = null;
        if (data[i].startsWith("#")) {
            temp = data[i].split("=")[1].replace('{', "").replace('}', "").replace(" ", ""); //read value

            //console.log(data[i].split("=")[0].replace("#", "").replace(" ", ""));

            switch (data[i].split("=")[0].replace("#", "").replace(" ", "")) {
                case "worldPosition":
                    vec3.set(worldCoord, parseFloat(temp.split(",")[0]), parseFloat(temp.split(",")[1]), parseFloat(temp.split(",")[2]));
                    break;
                case "ambientReflCoef":
                    ambientReflCoef = parseFloat(temp);
                    break;
                case "diffuseReflCoef":
                    diffuseReflCoef = parseFloat(temp);
                    break;
                case "specularReflCoef":
                    specularReflCoef = parseFloat(temp);
                    break;
                case "initialRotation":
                    vec3.set(initialRotation, parseFloat(temp.split(",")[0]), parseFloat(temp.split(",")[1]), parseFloat(temp.split(",")[2]));
                    break;
                case "shininessCoef":
                    shininessCoef = parseFloat(temp);
                    break;
                case "autoRotate":
                    temp = temp.replace(/[^A-Za-z]/g, "") //remove annoying characters
                    autoRotate = (temp === "true");
                    break;
                case "rotationSpeed":
                    rotationSpeed = parseFloat(temp);
                    break;
                case "meshNo":
                    meshNo = parseInt(temp);
                    break;
                case "userRotation":
                    temp = temp.replace(/[^A-Za-z]/g, "") //remove annoying characters
                    userRotation = (temp === "true");
                    break;
                case "isLightSource":
                    temp = temp.replace(/[^A-Za-z]/g, "") //remove annoying characters
                    isLightSource = (temp === "true");
                    break;
                case "lightIntensity":
                    vec3.set(lightIntensity, parseFloat(temp.split(",")[0]), parseFloat(temp.split(",")[1]), parseFloat(temp.split(",")[2]));
                    break;      
                default:
                    break;
            }

            //console.log(data[i].split("=")[1]);
        }
    }

    //console.log(worldCoord);
    //console.log(ambientReflCoef);
    //console.log(diffuseReflCoef);
    //console.log(specularReflCoef);

    return new ezData(worldCoord, ambientReflCoef, diffuseReflCoef, specularReflCoef, initialRotation, shininessCoef, autoRotate, rotationSpeed, meshNo, userRotation,isLightSource,lightIntensity);
}


//ezData object
function ezData(worldCoord, ambientReflCoef, diffuseReflCoef, specularReflCoef, initialRotation, shininessCoef, autoRotate, rotationSpeed, meshNo, userRotation, isLightSource, lightIntensity) {
        this.worldCoord = worldCoord,
        this.ambientReflCoef = ambientReflCoef,
        this.diffuseReflCoef = diffuseReflCoef,
        this.specularReflCoef = specularReflCoef,
        this.initialRotation = initialRotation,
        this.shininessCoef = shininessCoef,
        this.autoRotate = autoRotate,
        this.rotationSpeed = rotationSpeed,
        this.meshNo = meshNo,
        this.userRotation = userRotation,
        this.isLightSource = isLightSource,
        this.lightIntensity = lightIntensity
};