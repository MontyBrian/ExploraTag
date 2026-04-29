<script setup>
import SmartTagTable from "@/components/SmartTagTable.vue";
import { ref, onMounted } from "vue";
import { useRouter } from "vue-router";
import { userManager, signOutRedirect } from "../services/auth.js";

const router = useRouter();

const username = ref("");
const isLogged = ref(false);
const loaded = ref(false);

const isSidebarOpen = ref(true);
const isUsermenuOpen = ref(false);

const toggleSidebar = () => {
  isSidebarOpen.value = !isSidebarOpen.value;
};

const toggleUserbutton = () => {
  isUsermenuOpen.value = !isUsermenuOpen.value;
};

const goToLogin = () => {
  isUsermenuOpen.value = false;
  router.push("/login");
};

const doLogout = async () => {
  isUsermenuOpen.value = false;
  await signOutRedirect();
};

const goToUserScreen = () => {
  isUsermenuOpen.value = false;
  router.push("/profile");
};

const goBackToHome = () => {
  isUsermenuOpen.value = false;
  router.push("/");
};

onMounted(async () => {
  try {
    const search = window.location.search;
    const hasCode = search.includes("code=");
    const hasState = search.includes("state=");

    if (hasCode && hasState) {
      await userManager.signinRedirectCallback();
      window.history.replaceState({}, document.title, "/");
    }

    const user = await userManager.getUser();
    console.log("loaded user:", user);
    console.log("user.profile:", user?.profile);

    if (user && !user.expired) {
      username.value =
          user.profile?.preferred_username ||
          user.profile?.["cognito:username"] ||
          user.profile?.name ||
          user.profile?.email ||
          user.username ||
          "Unbekannt";

      isLogged.value = true;
    } else {
      username.value = "";
      isLogged.value = false;
    }
  } catch (err) {
    console.error("Dashboard Fehler:", err);
    username.value = "";
    isLogged.value = false;
  } finally {
    loaded.value = true;
  }
});
</script>

<template>
  <div class="d-flex vh-100">
    <button class="btn btn-secondary toggle-btn border" @click="toggleSidebar">
      <i class="bi bi-list"></i>
    </button>

    <div class="sidebar" :class="{ closed: !isSidebarOpen }">
      <div class="content">
        <SmartTagTable/>
      </div>

      <div class="d-flex p-3 footer">
        <div class="dropdownmenu">
          <button class="userbutton btn btn-secondary dropdown-toggle" @click="toggleUserbutton">
            <template v-if="loaded">
              {{ isLogged ? `Hallo, ${username}` : "Nicht angemeldet" }}
            </template>
            <template v-else>
              Lade...
            </template>
          </button>

          <div v-show="isUsermenuOpen">
            <div class="userdropdown dropdown-menu show">
              <div v-if="isLogged" class="dropdown-item" @click="goToUserScreen">
                Profil
              </div>
              <div v-if="isLogged" class="dropdown-divider"></div>
              <div v-if="!isLogged" class="dropdown-item" @click="goToLogin">
                Anmelden
              </div>
              <div v-else class="dropdown-item" @click="doLogout">
                Abmelden
              </div>
            </div>
          </div>
        </div>

        <div class="w-100 align-content-center text-center">
          <router-link to="/impressum">
            Impressum
          </router-link>
        </div>
      </div>
    </div>

    <div class="flex-grow-1 position-relative">
      <!-- Hauptbereich -->
    </div>
  </div>
</template>

<style scoped>
.sidebar {
  width: 300px;
  transition: transform 0.3s ease;
  background-color: #f8f9fa;
  border-right: 1px solid #ddd;
  display: flex;
  flex-direction: column;
  height: 100%;
}

.sidebar.closed {
  transform: translateX(-100%);
}

.toggle-btn {
  position: fixed;
  top: 10px;
  left: 10px;
  z-index: 10;
}

.content {
  margin-top: 3.5rem;
  flex-grow: 1;
  overflow: auto;
}

.footer {
  position: relative;
}

.userdropdown {
  position: absolute;
  bottom: 75%;
  left: 5%;
}

.userbutton {
  width: 180px;
  white-space: nowrap;
  overflow: hidden;
  text-overflow: ellipsis;
}

.dropdown-menu {
  min-width: 0;
  width: 180px;
}
</style>