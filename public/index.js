// Put all onload AJAX calls here, and event listeners
jQuery(document).ready(function() {
    // On page-load AJAX Example
    jQuery.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/endpoint1',   //The server endpoint we are connecting to
        data: {
            data1: "Value 1",
            data2:1234.56
        },
        success: function (data) {
            /*  Do something with returned object
                Note that what we get is an object, not a string, 
                so we do not need to parse it on the server.
                JavaScript really does handle JSONs seamlessly 
            */ 
            jQuery('#blah').html("On page load, received string '"+data.somethingElse+"' from server");
            //We write the object to the console to show that the request was successful
            console.log(data); 

        },
        fail: function(error) {
            // Non-200 return, do something with error
            $('#blah').html("On page load, received error from server");
            console.log(error); 
        }
    });

    $.ajax({
        type: 'get',
        dataType : 'json',
        url : '/serverFiles',
        data : {
            SVGData : null,
            errorMsgs : null
        },

        success: function (data) { //ERROR MESSAGES
            //console.log(data);

            let fileLog = new Array();

            let optionMenu = document.getElementById("chooseSVGFile");
            let shapeList = document.getElementById("shapeList");
            let scale = document.getElementById("scaleList");

            for(let i = 0; i < data.SVGData.length; i++){

                let newOption = document.createElement("option");
                newOption.setAttribute("value", data.SVGData[i].filename);
                newOption.text = data.SVGData[i].filename;
                optionMenu.append(newOption);
            }

            for(let i = 0; i < data.SVGData.length; i++){

                let newOption = document.createElement("option");
                newOption.setAttribute("value", data.SVGData[i].filename);
                newOption.text = data.SVGData[i].filename;
                shapeList.append(newOption);
            }

            for(let i = 0; i < data.SVGData.length; i++){

                let newOption = document.createElement("option");
                newOption.setAttribute("value", data.SVGData[i].filename);
                newOption.text = data.SVGData[i].filename;
                scale.append(newOption);
            }
            
            for(let i = 0; i < data.SVGData.length; i++){

                let currentData = new Array();

                currentData.push(data.SVGData[i].img);
                currentData.push(data.SVGData[i].filename);
                currentData.push(data.SVGData[i].fileSize);
                currentData.push(data.SVGData[i].numRect);
                currentData.push(data.SVGData[i].numCirc);
                currentData.push(data.SVGData[i].numPaths);
                currentData.push(data.SVGData[i].numGroups);

                fileLog.push(currentData);
            }

            fileLogPanel(fileLog);

        }
    });

    $("#chooseSVGFile").change(function(e) {

        let fileSelected = $(this).find(':selected').val();
        e.preventDefault();

        console.log(fileSelected);

        if(fileSelected == "Choose a file..."){
            alert("Choose a valid file");

        } else {

            $.ajax({
                type: 'get',
                dataType: 'json',
                url: 'SVGViewPanel',
                data: {
                    filename: fileSelected,
                    titleData: null,
                    descData: null,
                    attrData: null,
                    rectData: null,
                    circData: null,
                    pathData: null,
                    groupData: null
                },

                success: function (data) {
                    getSVGViewPanel(fileSelected, data.titleData, data.descData, data.attrData, data.rectData, data.circData, data.pathData, data.groupData);

                    let table = document.getElementById("components");
                    let compMenu = document.getElementById("componentList");
                    let compEditMenu = document.getElementById("componentListEdit");
                    compMenu.replaceChildren();
                    compEditMenu.replaceChildren();

                    let optionSel = "<option selected>Choose a component...</option>";
                    $("#componentList").html(optionSel);
                    $("#componentListEdit").html(optionSel);
                    
                    let svgImg = document.createElement("option");
                    svgImg.text = "SVG image";
                    compEditMenu.append(svgImg);

                    let rowsLen = table.rows.length;
                    for(let i = 1; i < rowsLen; i++){

                        let compName = table.rows[i].cells[0].innerHTML;

                        let option = document.createElement("option");
                        option.text = compName;
                        compMenu.append(option);
                    }

                    for(let i = 1; i < rowsLen; i++){

                        let compName = table.rows[i].cells[0].innerHTML;

                        let option = document.createElement("option");
                        option.text = compName;
                        compEditMenu.append(option);
                    }

                    console.log("hello");

                },

                fail: function (error) {
                    console.log(error);
                    console.log("ERROR: could not update SVG view panel");
                }
                
            });

        }

    });

    $("#submitComponentChoice").click(function(e) {
        let chosen = $("#showAttrselectComponentForm").find(':selected').val();
        let selectedFile = $("#chooseSVGFile").find(':selected').val();
        let strSplit = chosen.split(" ");
        e.preventDefault();

        //set type
        let type;
        if (strSplit[0] === "Circle") {
            type = 1

        } else if(strSplit[0] === "Rectangle"){
            type = 2;

        } else if(strSplit[0] === "Path"){
            type = 3;

        } else if(strSplit[0] === "Group"){
            type = 4;
        }

        if (chosen === "Choose Component...") {
            alert("Please choose a valid component from the list!");
            console.log("ERROR: no valid component was chosen");
        
        }else {
            $.ajax({
                type: 'get',
                dataType: 'json',
                url: 'otherAttributes',
                data: {
                    file: selectedFile, 
                    index: strSplit[1],
                    type: type, 
                    attrData: null
                }, 

                success: function (data) { //CHANGE
                    
                    let displayString = "Other attributes for " + chosen + "\n\n"; 
                    let modalbody = document.getElementById("modalBody");

                    //no other data to display
                    if (data.attrData.length === 0) {
                        displayString += "This component has no other data to display!";
                    }
                    
                    for (let i = 0; i < data.attrData.length; i++) {
                        displayString += data.attrData[i].name + ": " + data.attrData[i].value + "\n"; 
                    }
                    
                    // let p = document.createElement("p");
                    // p.text = displayString;
                    // modalbody.append(p);

                    alert(displayString);
                    console.log(data.otherDataArray);
                },
                fail: function (error) {
                    console.log(error);
                    console.log("ERROR: unable to obtain other data");
                }

            });
        }
    });

    $("#submitEditChoice").click(function(e) {
        let chosen = $("#showEditselectComponentForm").find(':selected').val();
        let selectedFile = $("#chooseSVGFile").find(':selected').val();
        let name = $("#attrName").val();
        let value = $("#attrVal").val();
        let strSplit = chosen.split(" ");
        e.preventDefault();

        //set type
        let type;
        if (strSplit[0] === "Circle") {
            type = 1

        } else if(strSplit[0] === "Rectangle"){
            type = 2;

        } else if(strSplit[0] === "Path"){
            type = 3;

        } else if(strSplit[0] === "Group"){
            type = 4;
            
        } else {

            type = 0;
        }

        if (chosen === "Choose Component..." || name === "" || value === "") {
            alert("Please choose a valid component and fill out all of the fields");
            console.log("ERROR: no valid component was chosen");
        
        } else {
            $.ajax({
                type: 'get',
                dataType: 'json',
                url: 'editAttr',
                data: {
                    file: selectedFile, 
                    index: strSplit[1],
                    type: type, 
                    name, name,
                    value, value,
                    msg : null,
                }, 

                success: function (data) { //CHANGE

                    alert(data.msg);
                    location.reload();
                    console.log("Result: " + data.message);
                    
                    
                    let modalbody = document.getElementById("modalBody");
                    
                    // let p = document.createElement("p");
                    // p.text = displayString;
                    // modalbody.append(p);

                    alert(displayString);
                    console.log(data.otherDataArray);
                },
                fail: function (error) {
                    console.log(error);
                    console.log("ERROR: unable to obtain other data");
                }

            });
        }
    });

    $("#submitNameChoice").click(function(e) {
        let chosen = $("#showNameSelectForm").find(':selected').val();
        let selectedFile = $("#chooseSVGFile").find(':selected').val();
        let newName = $("#newName").val();
        e.preventDefault();

        if(newName.length > 255){
            alert("Name is too long (must be less than 256 character)");
            console.log("ERROR: name too long");

        } else if (chosen === "Choose Title or Description...") {
            alert("Please choose a valid option.");
            console.log("ERROR: no valid option was chosen");
        
        } else if (selectedFile === "Choose a file...") {
            alert("Please choose a file.");
            console.log("ERROR: no valid file was chosen");
        
        } else {
            $.ajax({
                type: 'get',
                dataType: 'json',
                url: 'rename',
                data: {
                    file: selectedFile, 
                    chosen: chosen,
                    name: newName.trim(),
                    msg: null
                }, 

                success: function (data) { //CHANGE
                
                    alert(data.msg);

                    if (data.msg === "Success") {
                        location.reload();
                    }
                    
                    console.log("new name " + data.msg);
                    
                },
                fail: function (error) {
                    console.log(error);
                    console.log("ERROR: unable to obtain other data");
                }

            });
        }
    });

    $("#createSVGForm").submit(function(e) {

        let file = $("#fileName").val();
        let title = $("#title1").val();
        let desc = $("#description").val();
        e.preventDefault();

        let svgtoJSON = JSON.stringify({"title": title, "desc": desc});

        let validFile = file.split('.').pop();
        if(validFile !== "svg"){

            alert("ERROR: Invalid filename (must contain .svg)");
            console.log("ERROR: name too long");

        } else if(title.length > 255 || desc.length > 255){

            alert("ERROR: title and description must be less that 256 characters");
            console.log("ERROR: name too long");

        } else {

            //ajax call to talk to server
            $.ajax({
                type: 'get',
                dataType: 'json',
                url: 'createSVG',
                data: {
                    file: file,
                    title: title,
                    description: desc,
                    message: null,
                },

                success: function (data) {
                    alert(data.message);
                    
                    if (data.message === "Success: New SVG image created.") {
                        location.reload();  
                    }

                    console.log(data);
                },
                fail: function (error) {
                    console.log(error);
                    console.log("ERROR: unable to create new doc");
                }
            });

        }

    });

    $("#submitAddChoice").click(function(e) {

        if ( $("#shapeList").find(':selected').val() === "Choose File..." || $("#shapes").find(':selected').val() === "Choose a Shape...") {
            alert("Please choose a file and shape");
        
        } else {

            let file = $("#shapeList").find(':selected').val();
            let shape = $("#shapes").find(':selected').val();
            let shapeType = -1;

            let addShape = document.getElementById("addShape");
            addShape.replaceChildren();

            if(shape === "Circle"){
                shapeType = 0;
                for(let i = 0; i < 4; i++){

                    let newInput = document.createElement("input");
                    newInput.setAttribute("type", "number");
                    newInput.setAttribute("step", "any");

                    if(i == 0){
                        newInput.setAttribute("placeholder", "cx");
                        newInput.setAttribute("id", "cx");
                    }

                    if(i == 1){
                        newInput.setAttribute("placeholder", "cy");
                        newInput.setAttribute("id", "cy");
                    }

                    if(i == 2){
                        newInput.setAttribute("placeholder", "r");
                        newInput.setAttribute("min", "0");
                        newInput.setAttribute("id", "r");
                    }

                    if(i == 3){
                        newInput.setAttribute("placeholder", "units");
                        newInput.setAttribute("id", "units");
                        newInput.setAttribute("type", "text");
                    }

                    addShape.append(newInput);
                }

            } else {

                shapeType = 1;

                for(let i = 0; i < 5; i++){

                    let newInput = document.createElement("input");
                    newInput.setAttribute("type", "number");
                    newInput.setAttribute("step", "any");

                    if(i == 0){
                        newInput.setAttribute("placeholder", "x");
                        newInput.setAttribute("id", "x");
                    }

                    if(i == 1){
                        newInput.setAttribute("placeholder", "y");
                        newInput.setAttribute("id", "y");
                    }

                    if(i == 2){
                        newInput.setAttribute("placeholder", "width");
                        newInput.setAttribute("id", "w");
                        newInput.setAttribute("min", "0");
                    }

                    if(i == 3){
                        newInput.setAttribute("placeholder", "height");
                        newInput.setAttribute("id", "h");
                        newInput.setAttribute("min", "0");
                    }

                    if(i == 4){

                        newInput.setAttribute("type", "text");
                        newInput.setAttribute("placeholder", "units");
                        newInput.setAttribute("id", "units");
                    }

                    addShape.append(newInput);
                }

            }

            $("#addModal").modal('toggle');
            $("#addSubmitModal").modal('toggle');

            //adding the route to the given file
            $("#submitShapeChoice").click(function(e) {

                if (shape == -1) {
                    alert("Choose valid Shape");
                
                } else {

                    if (file !== "Choose a file...") {

                        let json;
                        let x, y, width, height;
                        let cx, cy, r;
                        let units;
                        

                        if(shapeType == 0){

                            cx = +$("#cx").val();
                            cy = +$("#cy").val();
                            r = +$("#r").val();
                            units = $("#units").val();

                            json = JSON.stringify({"cx": cx, "cy": cy, "r": r, "units": units});

                        } else {

                            x = +$("#x").val();
                            y = +$("#y").val();
                            width = +$("#w").val();
                            height = +$("#h").val();
                            units = $("#units").val();
                            json = JSON.stringify({"x": x, "y": y, "w": width, "h": height , "units": units});
                        }

                        if(width < 0 || height < 0 || r < 0){
                            alert("ERROR: Please enter valid units");

                        } else {

                            $.ajax({
                                type: 'get',
                                dataType: 'json',
                                url: 'addShape',
                                data: {
                                    filename: file,
                                    type: shapeType,
                                    jsonStr: json, 
                                    message: null
                                },
                
                                success: function (data) {
                                    alert(data.message);
                
                                    //reload the page to update tables
                                    location.reload();
                                    console.log("Result: " + data.message);
                                },
                                fail: function (error) {
                                    console.log(error);
                                    console.log("ERROR: unable to add shape");
                                }
                            });
                        }
                    }

                }
            });
        }
    });

    $("#submitScaleChoice").click(function(e) {

        if ( $("#scaleList").find(':selected').val() === "Choose File..." || $("#scaleShapes").find(':selected').val() === "Choose a Shape...") {
            alert("Please choose a file and shape");
        
        } else if ($("#scaleSize").val() <= 0) {

            alert("Please choose valid scale size");

        } else {

            if (file !== "Choose a file...") {

                let file = $("#scaleList").find(':selected').val();
                let shape = $("#scaleShapes").find(':selected').val();
                let scale = $("#scaleSize").val();
                let shapeType = -1;

                console.log(file);
                console.log(scale);

                if(shape === "Circle"){

                    shapeType = 1;

                } else {

                    shapeType = 0;
                }

                $.ajax({
                    type: 'get',
                    dataType: 'json',
                    url: 'scaleSVG',
                    data: {
                        filename: file,
                        type: shapeType,
                        scale: scale,
                        message: null
                    },
    
                    success: function (data) {
                        alert(data.message);
    
                        //reload the page to update tables
                        location.reload();
                        console.log("Result: " + data.message);
                    },
                    fail: function (error) {
                        console.log(error);
                        console.log("ERROR: unable to add shape");
                    }
                });
            }

        }
    });

    $("#upload").click(function(e) {

        let file = $("#uploadFile").find(':selected').val();
        console.log(file);
    });

});

function getSVGViewPanel(filename, title, desc, attrData, rectData, circData, pathData, groupData) {

    let imgTable = document.getElementById("svgView");
    let titdescTable = document.getElementById("tidesc");
    let componentTable = document.getElementById("components");

    let numRows = imgTable.rows.length;
    for (let i = numRows-1; i > 0; i--) {
        imgTable.deleteRow(i);
    }

    numRows = titdescTable.rows.length;
    for (let i = numRows-1; i > 0; i--) {
        titdescTable.deleteRow(i);
    }

    numRows = componentTable.rows.length;
    for (let i = numRows-1; i > 0; i--) {
        componentTable.deleteRow(i);
    }

    if (rectData.length == 0 && circData.length == 0 && pathData.length == 0 && groupData.length == 0 || filename == "uploads/Choose a file...") {
        let row = table.insertRow(-1);
        let cell = row.insertCell(0);
        $(cell).attr("colspan", "3");
        let msg = document.createTextNode("No components to display! Please choose a file!");
        cell.appendChild(msg);
    
    } else {

        let imgRow = imgTable.insertRow();
        let imgCell = imgRow.insertCell(0);
        let img = document.createElement("img");
        img.setAttribute("src", filename);
        img.setAttribute("width", "800px");
        img.setAttribute("object-fit", "cover");
        imgCell.append(img);


        let row = titdescTable.insertRow();
        row.setAttribute("style", "height: 50px");
        let cell = row.insertCell(0);
        let newTitle = document.createTextNode(title);
        cell.append(newTitle);

        cell = row.insertCell(1);
        let newDesc = document.createTextNode(desc);
        cell.append(newDesc);

        console.log(title);

        let num = 1;

        rectData.forEach(currentRect => {

            let i = 0;

            let row = componentTable.insertRow();
            let cell = row.insertCell(i);

            let component = document.createTextNode("Rectangle " + num);
            cell.append(component);

            cell = row.insertCell(i + 1);
            let summary = document.createTextNode("Upper left corner: x = " + currentRect.x + currentRect.units + ", " + "y = " + currentRect.y + currentRect.units + ", " + "Width: " + currentRect.w +  currentRect.units + ", " + "Height: " + currentRect.h + currentRect.units );
            cell.append(summary);

            cell = row.insertCell(i + 2);
            let otherAtt = document.createTextNode(currentRect.numAttr);
            cell.append(otherAtt);

            num++;

        });

        num = 1;

        circData.forEach(currentCirc => {

            let i = 0;

            let row = componentTable.insertRow();
            let cell = row.insertCell(i);

            let component = document.createTextNode("Circle " + num);
            cell.append(component);

            cell = row.insertCell(i + 1);
            let summary = document.createTextNode("Circle: x = " + currentCirc.cx + currentCirc.units + ", " + "y = " + currentCirc.cy + currentCirc.units + ", " + "radius: " + currentCirc.r +  currentCirc.units);
            cell.append(summary);

            cell = row.insertCell(i + 2);
            let otherAtt = document.createTextNode(currentCirc.numAttr);
            cell.append(otherAtt);

            num++;

        });

        num = 1;
        pathData.forEach(currentPath => {

            let i = 0;

            let row = componentTable.insertRow();
            let cell = row.insertCell(i);

            let component = document.createTextNode("Path " + num);
            cell.append(component);

            cell = row.insertCell(i + 1);
            let summary = document.createTextNode("Path data = " + currentPath.d);
            cell.append(summary);

            cell = row.insertCell(i + 2);
            let otherAtt = document.createTextNode(currentPath.numAttr);
            cell.append(otherAtt);

            num++;

        });

        num = 1;
        groupData.forEach(currentGroup => {

            let i = 0;

            let row = componentTable.insertRow();
            let cell = row.insertCell(i);

            let component = document.createTextNode("Group " + num);
            cell.append(component);

            cell = row.insertCell(i + 1);
            let summary = document.createTextNode(currentGroup.children + " child elements");
            cell.append(summary);

            cell = row.insertCell(i + 2);
            let otherAtt = document.createTextNode(currentGroup.numAttr);
            cell.append(otherAtt);

            num++;

        });
    }

}

function fileLogPanel(SVGArray){

    let fileLogTable = document.getElementById("fileLogTable");
    if(SVGArray.length == 0){

        let cell = fileLogTable.insertRow(-1).insertCell(0);
        cell.setAttribute("colspan", "7");
        cell.setAttribute("height", "100px");
        let inputMsg = document.createTextNode("No SVG files to display");
        cell.appendChild(inputMsg);

    } else {

        SVGArray.forEach(SVGObject => {

            let row = fileLogTable.insertRow();
    
            for(let i = 0; i < SVGObject.length; i++){
    
                let cell = row.insertCell(i);
    
                if(i === 0){
    
                    let link = document.createElement("a");
                    let img = document.createElement("img");
                    img.setAttribute("src", SVGObject[i]);
                    img.setAttribute("width", "200px");
                    img.setAttribute("download", SVGObject[i]);
                    link.appendChild(img);
                    link.href = "../uploads/"+SVGObject[i];
                    link.download = SVGObject[i];
                    cell.appendChild(link);
    
                } else if (i === 1){
    
                    let link = document.createElement("a");
                    let filename = document.createTextNode(SVGObject[i]);
                    link.appendChild(filename);
                    link.href = "../uploads/"+SVGObject[i];
                    link.download = SVGObject[i];
                    cell.appendChild(link);
    
                } else if(i === 2){

                    let data = document.createTextNode(SVGObject[i] + "KB");
                    cell.appendChild(data);

                } else {
    
                    let data = document.createTextNode(SVGObject[i]);
                    cell.appendChild(data);
    
                }
    
            }
            
        });

    }

}