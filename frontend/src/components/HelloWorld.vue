<script setup>
</script>

<template>
  <div class="col-12 col-md-6">
    <h3 class="display-6">Hallo, Test!</h3> <br/>
    <div class="card">
      <div class="card-header">
        Geräte
      </div>

      <ul class="list-group list-group-flush">

        <li class="list-group-item">

          <div class="d-flex justify-content-between align-items-start w-100">

            <div class="flex-grow-1 border rounded p-2 me-3">

              <div class="d-flex align-items-center gap-3">
                <strong>Testname</strong>
                <small class="text-muted">ID: 12345</small>
              </div>

              <div class="mt-2 d-flex align-items-center gap-2">
                <small>Koordinaten: 1 1</small>
              </div>

            </div>

            <div class="d-flex flex-column gap-2">
              <button class="btn btn-sm btn-primary">
                <i class="bi bi-pencil-square"></i>
              </button>
              <button class="btn btn-sm btn-danger">
                <i class="bi bi-trash3-fill"></i>
              </button>
            </div>
          </div>

        </li>

      </ul>

      <div class="card-body">
        <button class="btn btn-success w-100">+ Gerät hinzufügen</button>
      </div>
    </div>
  </div>

</template>

<style scoped>

</style>
