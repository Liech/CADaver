result = cadaver({"type": "ClearProject"})
assert result.get("status") == "Success", f"ClearProject failed: {result}"
