<script>
const goal = 176000; // goal amount
let total = 0;

// calculate total from table
document.querySelectorAll("td").forEach(cell => {
  const val = parseFloat(cell.textContent.replace(".", "").replace(",", "."));
  if (!isNaN(val)) total += val * 1000; // convert 1.000 to 1000
});

const progress = Math.min((total / goal) * 100, 100);
document.getElementById("progressFill").style.width = progress + "%";
document.getElementById("progressText").textContent =
  `Rp${total.toLocaleString()} / Rp${goal.toLocaleString()} (${progress.toFixed(1)}%)`;
</script>
