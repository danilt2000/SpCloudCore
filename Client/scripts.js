function fetchAndDisplayAppNamesForDelete() {
    const myHeaders = new Headers();
    myHeaders.append("Authorization", getCookie("auth_token"));

    const requestOptions = {
        method: "GET",
        headers: myHeaders,
        redirect: "follow"
    };

    fetch("https://spcloudcore.almavid.ru/apps", requestOptions)
        .then((response) => response.json())
        .then((result) => {
            const apps = result.documents;
            const appNameSelect = document.getElementById('deleteAppName');

            // Clear existing options
            appNameSelect.innerHTML = '<option value="" disabled selected>Выберите приложение</option>';

            // Populate the select dropdown with app names
            apps.forEach(app => {
                const option = document.createElement('option');
                option.value = app.name;
                option.textContent = app.name;
                appNameSelect.appendChild(option);
            });
        })
        .catch((error) => console.error('Error:', error));
}

// Open Delete Modal and populate app names
document.querySelector('.delete-button').addEventListener('click', function () {
    fetchAndDisplayAppNamesForDelete();
    document.getElementById('deleteModal').style.display = "block";
});

// Close Delete Modal
document.querySelector('#deleteModal .close').onclick = function () {
    document.getElementById('deleteModal').style.display = "none";
}

// Handle Deletion
document.getElementById("deleteBtn").addEventListener("click", function () {
    showLoading();


    var deleteAppName = document.getElementById("deleteAppName").value;

    if (!deleteAppName) {
        alert("Please select an application to delete.");
        return;
    }

    var userConfirmed = confirm("Are you sure you want to delete the application '" + deleteAppName + " ?");
    
    if (!userConfirmed) {

        alert("Application deletion cancelled.");

        return;
    }

    const myHeaders = new Headers();
    myHeaders.append("Authorization", getCookie("auth_token"));

    const formdata = new FormData();
    formdata.append("UserId", getCookie("discord_id"));
    formdata.append("Name", deleteAppName);

    const requestOptions = {
        method: "DELETE",
        headers: myHeaders,
        body: formdata,
        redirect: "follow"
    };

    fetch("https://spcloudcore.almavid.ru/delete", requestOptions)
        .then((response) => response.text())
        .then((result) => {
            hideLoading();

            console.log(result);
            alert(result);

            fetchAndDisplayAppData();

            document.getElementById('deleteModal').style.display = "none"; // Close the modal after successful deletion
        })
        .catch((error) => {
            hideLoading();
            console.error(error);
            alert("Error deleting application.");
        });
});

































function showLoading() {
    document.getElementById('globalLoading').style.display = 'flex';
}

function hideLoading() {
    document.getElementById('globalLoading').style.display = 'none';
}
// Get the modal
function fetchAndDisplayAppNames() {
    const myHeaders = new Headers();
    myHeaders.append("Authorization", getCookie("auth_token"));

    const requestOptions = {
        method: "GET",
        headers: myHeaders,
        redirect: "follow"
    };

    fetch("https://spcloudcore.almavid.ru/apps", requestOptions)
        .then((response) => response.json())
        .then((result) => {
            const apps = result.documents;
            const appNameSelect = document.getElementById('updateAppName');

            // Clear existing options
            appNameSelect.innerHTML = '<option value="" disabled selected>Выберите приложение</option>';

            // Populate the select dropdown with app names
            apps.forEach(app => {
                const option = document.createElement('option');
                option.value = app.name;
                option.textContent = app.name;
                appNameSelect.appendChild(option);
            });
        })
        .catch((error) => console.error('Error:', error));
}
















// Get the update modal
var updateModal = document.getElementById("updateModal");

// Get the button that opens the update modal
var updateBtnOpen = document.querySelector('.update-button');

document.querySelector('.update-button').addEventListener('click', function () {
    fetchAndDisplayAppNames();
    document.getElementById('updateModal').style.display = "block";
});

// Get the <span> element that closes the update modal
var updateSpanClose = updateModal.querySelector('.close');

// When the user clicks the button, open the update modal 
updateBtnOpen.onclick = function () {
    updateModal.style.display = "block";
}

// When the user clicks on <span> (x), close the update modal
updateSpanClose.onclick = function () {
    updateModal.style.display = "none";
}

// When the user clicks anywhere outside of the update modal, close it
window.onclick = function (event) {
    if (event.target == updateModal) {
        updateModal.style.display = "none";
    }
}

// Form submission logic for updating the application
document.getElementById("updateBtn").addEventListener("click", function () {

    showLoading();
    var updateFileInput = document.getElementById("updateFileInput");
    var updateAppName = document.getElementById("updateAppName").value;

    if (updateFileInput.files.length === 0) {
        alert("Please select a file.");
        return;
    }

    const myHeaders = new Headers();
    myHeaders.append("Authorization", getCookie("auth_token"));

    const formdata = new FormData();
    formdata.append("File", updateFileInput.files[0]);
    formdata.append("UserId", getCookie("discord_id"));
    formdata.append("Name", updateAppName);

    const requestOptions = {
        method: "PUT",
        headers: myHeaders,
        body: formdata,
        redirect: "follow"
    };

    fetch("https://spcloudcore.almavid.ru/update", requestOptions)
        .then((response) => response.text())
        .then((result) => {
            hideLoading();
            console.log(result);
            alert(result);
            fetchAndDisplayAppData();

            updateModal.style.display = "none";
        })
        .catch((error) => {
            hideLoading();
            console.error(error);
            alert("Error updating application.");
        });
});











































var modal = document.getElementById("uploadModal");

// Get the button that opens the modal
var btn = document.querySelector('.upload-button');

// Get the <span> element that closes the modal
var span = document.getElementsByClassName("close")[0];

// When the user clicks the button, open the modal 
btn.onclick = function () {
    modal.style.display = "block";
}

// When the user clicks on <span> (x), close the modal
span.onclick = function () {
    modal.style.display = "none";
}

// When the user clicks anywhere outside of the modal, close it
window.onclick = function (event) {
    if (event.target == modal) {
        modal.style.display = "none";
    }
}





document.getElementById("uploadBtn").addEventListener("click", function () {
    showLoading();
    var fileInput = document.getElementById("fileInput");
    var appName = document.getElementById("appName").value;
    var appTarget = document.getElementById("appTarget").value;

    if (fileInput.files.length === 0) {
        alert("Please select a file.");
        return;
    }

    const myHeaders = new Headers();
    myHeaders.append("Authorization", getCookie("auth_token"));

    const formdata = new FormData();
    formdata.append("File", fileInput.files[0]);
    formdata.append("UserId", getCookie("discord_id"));
    formdata.append("Name", appName);
    formdata.append("Target", appTarget);

    const requestOptions = {
        method: "POST",
        headers: myHeaders,
        body: formdata,
        redirect: "follow"
    };

    fetch("https://spcloudcore.almavid.ru/publish", requestOptions)
        .then((response) => response.text())
        .then((result) => {
            hideLoading();

            alert(result);

            fetchAndDisplayAppData();

            modal.style.display = "none";
        })
        .catch((error) => {
            hideLoading();

            console.error(error);
            alert("Error uploading file.");
        });
});











function getQueryParam(param) {
    const urlParams = new URLSearchParams(window.location.search);
    return urlParams.get(param);
}

const code = getQueryParam('code');

if (code) {
    const myHeaders = new Headers();
    myHeaders.append("Content-Type", "application/json");

    const raw = JSON.stringify({
        "code": code
    });

    const requestOptions = {
        method: "POST",
        headers: myHeaders,
        body: raw,
        redirect: "follow"
    };

    fetch("https://spcloudcore.almavid.ru/login", requestOptions)
        .then(response => response.json())
        .then(result => {
            const discordId = result.document.discord_id;
            const authToken = result.document.auth_token;
            const username = result.document.username;

            setCookie("discord_id", discordId, 730);
            setCookie("auth_token", authToken, 730);
            setCookie("username", username, 730);

            console.log("Cookies set: discord_id and auth_token");

            displayUsername(username);

        })
        .catch(error => console.error('Error:', error));
}

function setCookie(name, value, days) {
    const date = new Date();
    date.setTime(date.getTime() + (days * 24 * 60 * 60 * 1000)); // Convert days to milliseconds
    const expires = "expires=" + date.toUTCString();
    document.cookie = name + "=" + value + ";" + expires + ";path=/";
}

function getCookie(name) {
    const value = `; ${document.cookie}`;
    const parts = value.split(`; ${name}=`);
    if (parts.length === 2) return parts.pop().split(';').shift();
}

function fetchAndDisplayAppData() {
    const myHeaders = new Headers();
    myHeaders.append("Authorization", getCookie("auth_token"));

    const requestOptions = {
        method: "GET",
        headers: myHeaders,
        redirect: "follow"
    };

    fetch("https://spcloudcore.almavid.ru/apps", requestOptions)
        .then((response) => response.json())
        .then((result) => {
            const apps = result.documents;
            const table = document.querySelector('.server-stats table');

            // Clear existing table rows (except headers)
            table.querySelectorAll('tr:not(:first-child)').forEach(row => row.remove());

            // Loop through each app and create a new table row
            apps.forEach(app => {
                const row = table.insertRow();

                const nameCell = row.insertCell(0);
                const urlCell = row.insertCell(1);
                const cpuUsageCell = row.insertCell(2);
                const memoryUsageCell = row.insertCell(3);
                const targetCell = row.insertCell(4);

                // Populate cells with app data
                nameCell.textContent = app.name; // Ensure the name is displayed
                urlCell.textContent = app.url;
                cpuUsageCell.textContent = "N/A";  // Placeholder as no CPU usage info is provided
                memoryUsageCell.textContent = "N/A";  // Placeholder as no Memory usage info is provided
                targetCell.textContent = app.target;  // Assuming all services are active as status is not provided
            });
        })
        .catch((error) => console.error('Error:', error));
}

fetchAndDisplayAppData();

function displayUsername(username) {
    const usernameElement = document.querySelector('.username');
    if (usernameElement) {
        usernameElement.textContent = username;
    }
}

const storedUsername = getCookie("username");
if (storedUsername) {
    displayUsername(storedUsername);
}



