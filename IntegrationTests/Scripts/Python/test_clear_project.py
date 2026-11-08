result = cadaver({"type": "ClearProject", "version": 1})
assert result.get("status") == "Success", f"ClearProject failed: {result}"
