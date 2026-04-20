<script>
  import SmartTagTable from "@/components/SmartTagTable.vue";

  export default {
    components: {SmartTagTable, SmarttagManagement: SmartTagTable},
  data() {
  return {
  isOpen: true
}
},
  methods: {
  toggleSidebar() {
  this.isOpen = !this.isOpen
},
  toggleUserbutton(){

  }
}
}
</script>

<template>
  <div class="d-flex vh-100">

    <button class="btn btn-secondary toggle-btn border" @click="toggleSidebar">
      <i class="bi bi-list"></i>
    </button>

    <div class="sidebar" :class="{ closed: !isOpen }">
      <div class="content">
        <SmartTagTable/>
      </div>
      <div class="d-flex p-3 footer">
        <button class="userbutton btn btn-secondary w-100" @click="toggleUserbutton">
          Hallo, Test!
        </button>
        <div class="dropdown">

        </div>
        <div class="w-100 align-content-center text-center">
          <router-link to="/impressum">
            Impressum
          </router-link>
        </div>

      </div>
    </div>

    <!-- Hauptbereich -->
    <div class="flex-grow-1 position-relative">

      <!-- <Karte/> -->
    </div>

  </div>
</template>

<style>
.sidebar {
  width: 300px;
  transition: transform 0.3s ease;
  background-color: #f8f9fa;
  border-right: 1px solid #ddd;

  display: flex;
  flex-direction: column;
  height:100%;
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
</style>