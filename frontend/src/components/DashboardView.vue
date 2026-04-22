<script setup>
import {ref, onMounted} from "vue";
import {userManager,signOutRedirect} from "../services/auth.js";
import {useRouter} from "vue-router";

const router = useRouter();
const email = ref("");
const isLogged = ref(false);
const loaded = ref(false);

onMounted(async () => {
  const user = await userManager.getUser();
  console.log("loaded user:", user);

  if (user && !user.expired) {
    email.value = user.profile?.email || user.username || "Unbekannt";
    isLogged.value = true;
  } else {
    router.replace("/login");
  }

  loaded.value = true;
});
</script>

<template>
  <div style="padding: 20px; font-family: sans-serif;">
    <p v-if="!loaded">Lade Benutzer ...</p>

    <template v-else>
      <h1>Aktueller Status</h1>

      <div v-if="isLogged">
        <p>Willkommen, {{ email }}</p>
        <button @click="signOutRedirect">Logout</button>
      </div>

      <div v-else>
        <p>Nicht eingeloggt.</p>
      </div>
    </template>
  </div>
</template>