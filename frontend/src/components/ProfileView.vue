<template>
  <section class="profile-page">
    <div class="profile-card">
      <div class="profile-header">
        <div>
          <h1>Mein Profil</h1>
          <p>Hier kannst du deine persönlichen Daten ansehen und bearbeiten.</p>
        </div>
        <button class="btn btn-primary" @click="goBackToHome">
          Startseite
        </button>
        <button v-if="!editMode" class="btn btn-primary" @click="startEdit">
          Daten bearbeiten
        </button>
      </div>

      <div v-if="loading" class="info-box">Benutzerdaten werden geladen ...</div>
      <div v-else-if="errorMessage" class="info-box error">{{ errorMessage }}</div>

      <form v-else class="profile-form" @submit.prevent="saveProfile">
        <div class="grid">
          <div class="field">
            <label>Benutzer-ID</label>
            <input :value="profile.sub || ''" disabled />
          </div>

          <div class="field">
            <label for="name">Anzeigename</label>
            <input id="name" v-model="form.name" :disabled="!editMode" />
          </div>

          <div class="field">
            <label for="given_name">Vorname</label>
            <input id="given_name" v-model="form.given_name" :disabled="!editMode" />
          </div>

          <div class="field">
            <label for="family_name">Nachname</label>
            <input id="family_name" v-model="form.family_name" :disabled="!editMode" />
          </div>

          <div class="field">
            <label for="email">E-Mail</label>
            <input id="email" v-model="form.email" type="email" :disabled="!editMode" />
          </div>

          <div class="field">
            <label for="phone_number">Telefon</label>
            <input id="phone_number" v-model="form.phone_number" :disabled="!editMode" />
          </div>
        </div>

        <div v-if="successMessage" class="info-box success">{{ successMessage }}</div>

        <div v-if="editMode" class="actions">
          <button type="button" class="btn btn-secondary" @click="cancelEdit">Abbrechen</button>
          <button type="submit" class="btn btn-primary" :disabled="saving">
            {{ saving ? "Speichert ..." : "Speichern" }}
          </button>
        </div>
      </form>
    </div>
  </section>
</template>

<script setup>
import { onMounted, reactive, ref } from "vue";
import { userManager } from "../services/auth.js";
import { useRouter } from "vue-router";

const loading = ref(true);
const saving = ref(false);
const editMode = ref(false);
const errorMessage = ref("");
const successMessage = ref("");
const router = useRouter();

const goBackToHome = () => {
  router.push("/");
};

const profile = ref({});

const form = reactive({
  name: "",
  given_name: "",
  family_name: "",
  email: "",
  phone_number: ""
});

function mapProfileToForm(data) {
  form.name = data.name || "";
  form.given_name = data.given_name || "";
  form.family_name = data.family_name || "";
  form.email = data.email || "";
  form.phone_number = data.phone_number || "";
}

async function loadProfile() {
  loading.value = true;
  errorMessage.value = "";
  successMessage.value = "";

  try {
    const user = await userManager.getUser();

    if (!user || !user.access_token) {
      throw new Error("Kein eingeloggter Benutzer gefunden.");
    }

    const mergedProfile = {
      ...user.profile
    };

    profile.value = mergedProfile;
    mapProfileToForm(mergedProfile);
  } catch (error) {
    errorMessage.value = error.message || "Profil konnte nicht geladen werden.";
  } finally {
    loading.value = false;
  }
}

function startEdit() {
  successMessage.value = "";
  editMode.value = true;
}

function cancelEdit() {
  mapProfileToForm(profile.value);
  errorMessage.value = "";
  successMessage.value = "";
  editMode.value = false;
}

async function saveProfile() {
  saving.value = true;
  errorMessage.value = "";
  successMessage.value = "";

  try {
    const user = await userManager.getUser();

    if (!user || !user.access_token) {
      throw new Error("Kein Access Token vorhanden.");
    }

    const attributes = [
      { Name: "name", Value: form.name },
      { Name: "given_name", Value: form.given_name },
      { Name: "family_name", Value: form.family_name },
      { Name: "email", Value: form.email },
      { Name: "phone_number", Value: form.phone_number }
    ].filter(item => item.Value !== "");

    const response = await fetch(
        "https://cognito-idp.eu-central-1.amazonaws.com/",
        {
          method: "POST",
          headers: {
            "Content-Type": "application/x-amz-json-1.1",
            "X-Amz-Target": "AWSCognitoIdentityProviderService.UpdateUserAttributes",
            Authorization: `Bearer ${user.access_token}`
          },
          body: JSON.stringify({
            AccessToken: user.access_token,
            UserAttributes: attributes
          })
        }
    );

    if (!response.ok) {
      const errorData = await response.json();
      throw new Error(errorData.message || "Speichern fehlgeschlagen.");
    }

    profile.value = {
      ...profile.value,
      ...form
    };

    successMessage.value = "Deine Profildaten wurden gespeichert.";
    editMode.value = false;
  } catch (error) {
    errorMessage.value = error.message || "Profil konnte nicht gespeichert werden.";
  } finally {
    saving.value = false;
  }
}

onMounted(loadProfile);
</script>

<style scoped>
.profile-page {
  padding: 2rem;
  display: flex;
  justify-content: center;
}

.profile-card {
  width: 100%;
  max-width: 900px;
  background: #ffffff;
  border-radius: 16px;
  padding: 2rem;
  box-shadow: 0 10px 30px rgba(0, 0, 0, 0.08);
}

.profile-header {
  display: flex;
  justify-content: space-between;
  align-items: start;
  gap: 1rem;
  margin-bottom: 2rem;
}

.profile-header h1 {
  margin: 0 0 0.5rem;
}

.profile-form .grid {
  display: grid;
  grid-template-columns: repeat(2, minmax(0, 1fr));
  gap: 1rem;
}

.field {
  display: flex;
  flex-direction: column;
}

.field label {
  font-weight: 600;
  margin-bottom: 0.35rem;
}

.field input {
  border: 1px solid #d0d7de;
  border-radius: 10px;
  padding: 0.8rem 0.95rem;
  font-size: 1rem;
}

.field input:disabled {
  background: #f4f6f8;
  color: #555;
}

.actions {
  display: flex;
  justify-content: flex-end;
  gap: 0.75rem;
  margin-top: 1.5rem;
}

.btn {
  border: none;
  border-radius: 10px;
  padding: 0.85rem 1.1rem;
  font-weight: 600;
  cursor: pointer;
}

.btn-primary {
  background: #0f766e;
  color: white;
}

.btn-secondary {
  background: #e5e7eb;
  color: #111827;
}

.info-box {
  padding: 1rem;
  border-radius: 10px;
  margin-bottom: 1rem;
  background: #f3f4f6;
}

.info-box.success {
  background: #dcfce7;
  color: #166534;
}

.info-box.error {
  background: #fee2e2;
  color: #991b1b;
}

@media (max-width: 700px) {
  .profile-form .grid {
    grid-template-columns: 1fr;
  }

  .profile-header {
    flex-direction: column;
  }
}
</style>