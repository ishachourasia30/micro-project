function showPage(pageName) {
    let pages = document.querySelectorAll(".page");

    pages.forEach(function(page) {
        page.classList.add("hidden");
    });

    document.getElementById(pageName).classList.remove("hidden");

    const steps = {
        add: "Step 1 of 7",
        view: "Step 2 of 7",
        search: "Step 3 of 7",
        update: "Step 4 of 7",
        order: "Step 5 of 7",
        track: "Step 6 of 7",
        exit: "Step 7 of 7",
        menu: "Main Menu"
    };

    document.getElementById("currentStep").textContent =
        steps[pageName];
}

function showResult(elementId, message, isHTML = false) {
    const element = document.getElementById(elementId);

    element.classList.remove("hidden");

    if (isHTML) {
        element.innerHTML = message;
    } else {
        element.textContent = message;
    }
}

function showConfirmation(elementId) {
    document.getElementById(elementId).classList.remove("hidden");
}

function hideConfirmation(elementId) {
    document.getElementById(elementId).classList.add("hidden");
}

async function sendPost(url, values) {
    const response = await fetch(url, {
        method: "POST",
        headers: {
            "Content-Type": "application/x-www-form-urlencoded"
        },
        body: new URLSearchParams(values)
    });

    const message = await response.text();

    if (!response.ok) {
        throw new Error(message);
    }

    return message;
}

async function sendGet(url) {
    const response = await fetch(url);
    const message = await response.text();

    if (!response.ok) {
        throw new Error(message);
    }

    return message;
}

// MAIN MENU
document.getElementById("menuButton").addEventListener("click", function() {
    showPage("menu");
});

document.querySelectorAll("[data-page]").forEach(function(button) {
    button.addEventListener("click", function() {
        showPage(button.dataset.page);
    });
});

document.querySelectorAll(".backButton").forEach(function(button) {
    button.addEventListener("click", function() {
        showPage("menu");
    });
});

document.querySelectorAll(".nextButton").forEach(function(button) {
    button.addEventListener("click", function() {
        showPage(button.dataset.next);
    });
});

// 1. ADD PRODUCT
document.getElementById("addForm").addEventListener("submit", async function(event) {
    event.preventDefault();

    hideConfirmation("addConfirmation");

    try {
        const message = await sendPost("/api/add", {
            name: document.getElementById("productName").value,
            price: document.getElementById("productPrice").value,
            quantity: document.getElementById("productQuantity").value
        });

        showResult("addResult", message);
        showConfirmation("addConfirmation");

        this.reset();
    }
    catch (error) {
        showResult("addResult", "Error: " + error.message);
    }
});

// 2. VIEW PRODUCTS
document.getElementById("viewButton").addEventListener("click", async function() {
    hideConfirmation("viewConfirmation");

    try {
        const result = await sendGet("/api/view");

        showResult("viewResult", result, true);
        showConfirmation("viewConfirmation");
    }
    catch (error) {
        showResult("viewResult", "Error: " + error.message);
    }
});

// 3. SEARCH PRODUCT
document.getElementById("searchForm").addEventListener("submit", async function(event) {
    event.preventDefault();

    hideConfirmation("searchConfirmation");

    try {
        const result = await sendPost("/api/search", {
            id: document.getElementById("searchId").value
        });

        showResult("searchResult", result, true);
        showConfirmation("searchConfirmation");
    }
    catch (error) {
        showResult("searchResult", "Error: " + error.message);
    }
});

// 4. UPDATE QUANTITY
document.getElementById("updateForm").addEventListener("submit", async function(event) {
    event.preventDefault();

    hideConfirmation("updateConfirmation");

    try {
        const result = await sendPost("/api/update", {
            id: document.getElementById("updateId").value,
            quantity: document.getElementById("newQuantity").value
        });

        showResult("updateResult", result);
        showConfirmation("updateConfirmation");

        this.reset();
    }
    catch (error) {
        showResult("updateResult", "Error: " + error.message);
    }
});

// 5. PLACE ORDER
document.getElementById("orderForm").addEventListener("submit", async function(event) {
    event.preventDefault();

    hideConfirmation("orderConfirmation");

    const confirmed = confirm("Are you sure you want to place this order?");

    if (!confirmed) {
        return;
    }

    try {
        const result = await sendPost("/api/order", {
            id: document.getElementById("orderId").value,
            quantity: document.getElementById("orderQuantity").value
        });

        showResult("orderResult", result);
        showConfirmation("orderConfirmation");

        this.reset();
    }
    catch (error) {
        showResult("orderResult", "Error: " + error.message);
    }
});

// 6. TRACK ORDERS
document.getElementById("trackButton").addEventListener("click", async function() {
    hideConfirmation("trackConfirmation");

    try {
        const result = await sendGet("/api/track");

        showResult("trackResult", result, true);
        showConfirmation("trackConfirmation");
    }
    catch (error) {
        showResult("trackResult", "Error: " + error.message);
    }
});

// START FROM ADD PRODUCT
showPage("add");
